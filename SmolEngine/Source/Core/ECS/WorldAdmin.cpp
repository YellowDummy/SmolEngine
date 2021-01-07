#include "stdafx.h"
#include "WorldAdmin.h"

#include "Core/ECS/ComponentsCore.h"
#include "Core/Renderer/Framebuffer.h"
#include "Core/Renderer/Renderer2D.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/Camera.h"
#include "Core/Renderer/Text.h"

#include "Core/AssetManager.h"
#include "Core/ImGui/EditorConsole.h"
#include "Core/Events/MouseEvent.h"
#include "Core/UI/UIButton.h"
#include "Core/Events/ApplicationEvent.h"
#include "Core/Physics2D/Box2D/CollisionListener2D.h"
#include "Core/Scripting/BehaviourPrimitive.h"

#include "Core/ECS/Systems/RendererSystem.h"
#include "Core/ECS/Systems/Physics2DSystem.h"
#include "Core/ECS/Systems/AudioSystem.h"
#include "Core/ECS/Systems/Animation2DSystem.h"
#include "Core/ECS/Systems/CameraSystem.h"
#include "Core/ECS/Systems/CommandSystem.h"
#include "Core/ECS/Systems/UISystem.h"
#include "Core/ECS/Systems/ScriptingSystem.h"

#include "Core/Animation/AnimationClip.h"

#include <filesystem>
#include <glm/glm.hpp>
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <rttr/registration.h>



namespace SmolEngine
{
	void WorldAdmin::Init()
	{
		m_InPlayMode = false;

#ifdef SMOLENGINE_EDITOR
		float aspectRatio = (float)Application::GetApplication().GetWindowWidth() / (float)Application::GetApplication().GetWindowHeight();
		m_EditorCamera = std::make_shared<EditorCameraController>(aspectRatio);
		m_EditorCamera->SetZoom(4.0f);
#endif
	}

	void WorldAdmin::StartGame()
	{
		if (LoadProjectConfig())
		{
			if (!LoadSceneRuntime(0))
			{
				NATIVE_ERROR("Couldn't load the scene!");
				abort();
			}
		}
	}

	void WorldAdmin::ShutDown()
	{

	}

	void WorldAdmin::OnPlay()
	{
		SceneData& sceneData = GetActiveScene().GetSceneData();
#ifdef SMOLENGINE_EDITOR
		if (!AssetManager::PathCheck(sceneData.m_filePath, sceneData.m_fileName))
		{
			NATIVE_ERROR("Failed to start the game!");
			return;
		}
		// We save the current scene before starting the simulation
		Save(sceneData.m_filePath);
#endif
		Box2DWorldSComponent* world = Box2DWorldSComponent::Get();

		// Setting Box2D Callbacks
		Physics2DSystem::OnBegin(world);

		// Creating rigidbodies and joints
		{
			const auto& view = sceneData.m_Registry.view<TransformComponent, Body2DComponent>();
			view.each([&](TransformComponent& tranform, Body2DComponent& body)
			{
					Physics2DSystem::CreateBody(&body, &tranform, world->World, GetActiveScene().FindActorByID(body.ActorID));
			});
		}

		// Finding which animation / audio clip should play on awake
		AudioSystem::OnAwake(sceneData.m_Registry, AudioEngineSComponent::Get()->Engine);
		Animation2DSystem::OnAwake(sceneData.m_Registry);

		// Sending start callback to all systems-scripts
		ScriptingSystem::OnSceneBegin(sceneData.m_Registry);
		m_InPlayMode = true;
	}

	void WorldAdmin::OnEndPlay()
	{
		m_InPlayMode = false;
		entt::registry& registry = GetActiveScene().m_SceneData.m_Registry;
		ScriptingSystem::OnSceneEnd(registry);

		// Deleting all Rigidbodies
		Physics2DSystem::DeleteBodies(registry, Box2DWorldSComponent::Get()->World);

		// Resetting Animation / Audio clips
		AudioSystem::OnReset(registry, AudioEngineSComponent::Get()->Engine);
		Animation2DSystem::OnReset(registry);
		AudioEngineSComponent::Get()->Engine->Reset();

#ifdef SMOLENGINE_EDITOR
		Load(GetActiveScene().m_SceneData.m_filePath);
#endif
	}

	void WorldAdmin::OnUpdate(DeltaTime deltaTime)
	{
		entt::registry& registry = GetActiveScene().m_SceneData.m_Registry;
#ifdef SMOLENGINE_EDITOR
		if (m_InPlayMode)
		{
			// Updating Phycics
			Physics2DSystem::OnUpdate(deltaTime, 6, 2, Box2DWorldSComponent::Get());
			// Sending OnProcess callback
			ScriptingSystem::OnSceneTick(registry, deltaTime);
		}
#else
		Box2DPhysicsSystem::OnUpdate(deltaTime, 6, 2, Box2DWorldSComponent::Get());
		// Sending OnProcess callback
		ScriptingSystem::OnSceneTick(registry, deltaTime);
#endif

		// Extracting Camera Tuples
		const auto& cameraGroup = registry.view<CameraBaseTuple, TransformComponent>();
		for (const auto& entity : cameraGroup)
		{
			auto& [tuple, transform] = cameraGroup.get<CameraBaseTuple, TransformComponent>(entity);

			// There is no need to render the scene if the camera is not our target or is disabled
			if (!tuple.Camera.isPrimaryCamera || !tuple.Camera.isEnabled) { continue; }

			// Calculating MVP
			CameraSystem::CalculateView(&tuple.Camera, &transform);

			// Rendering scene to target framebuffer
			RenderScene(tuple.Camera.ViewProjectionMatrix, FramebufferSComponent::Get()[0], false, &tuple.Camera, &transform);

			// At the moment we support only one viewport
			break;
		}

#ifndef SMOLENGINE_EDITOR
		// Rendering framebuffer to screen. We don't need to do this inside editor - it's handled by dear imgui
		Renderer2D::DrawFrameBuffer(FramebufferSComponent::Get()[0]->GetColorAttachmentID());
#endif
	}

	// TODO: Fix this event mess
	void WorldAdmin::OnEvent(Event& e)
	{
#ifndef  SMOLENGINE_EDITOR

		if (e.m_EventType == (int)EventType::S_WINDOW_RESIZE)
		{

			const auto& view = m_SceneData.m_Registry.view<CameraBaseTuple>();

			view.each([&](CameraBaseTuple& tuple)
				{
					CameraSystem::OnEvent(tuple, e);
				});

			auto& winResize = static_cast<WindowResizeEvent&>(e);

			for (const auto pair : FramebufferSComponent::Get())
			{
				const auto& [key, framebuffer] = pair;

				framebuffer->OnResize(winResize.GetWidth(), winResize.GetHeight());
			}
	     }
#endif 
		if (!m_InPlayMode) { return; }
		UISystem::OnEvent(GetActiveScene().m_SceneData.m_Registry, e);
	}

	void WorldAdmin::RenderScene(const glm::mat4& viewProjectionMatrix, Ref<Framebuffer> framebuffer, bool debugDrawEnabled,
		CameraComponent* targetCamera, TransformComponent* cameraTranform)
	{
		entt::registry& registry = GetActiveScene().m_SceneData.m_Registry;
#ifdef SMOLENGINE_EDITOR
		if (m_InPlayMode)
			Physics2DSystem::UpdateTransforms(registry);
#else
		Box2DPhysicsSystem::UpdateTransfroms(registry);

#endif
		RendererSystem::BeginDraw(viewProjectionMatrix, GetActiveScene().m_SceneData.m_AmbientStrength, framebuffer);
		{
			// 2D Textures 
			RendererSystem::Render2DTextures(registry);

			// 2D Light
			RendererSystem::Render2DLight(registry);

			// 2D Animations
			Animation2DSystem::Update(registry);
			RendererSystem::Render2DAnimations(registry);

			//UI
			if (targetCamera != nullptr && cameraTranform != nullptr)
				RendererSystem::RenderCanvases(registry, targetCamera, cameraTranform);
		}
		RendererSystem::EndDraw();

		if (debugDrawEnabled)
		{
			Renderer2D::BeginDebug();
			{
				RendererSystem::DebugDraw(registry);

#ifdef SMOLENGINE_EDITOR
				if (m_InPlayMode)
					ScriptingSystem::OnDebugDraw(registry);
#else
				ScriptingSystem::OnDebugDraw(registry);
#endif
			}
			Renderer2D::EndDebug();
		}
	}

	void WorldAdmin::ReloadAssets()
	{
		Scene& activeScene = GetActiveScene();
		entt::registry& registry = GetActiveScene().m_SceneData.m_Registry;

		activeScene.UpdateIDSet();

		// Updating AssetMap
		auto& assetMap = activeScene.m_SceneData.m_AssetMap;
		for (auto& pair : assetMap)
		{
			auto& [name, path] = pair;
			AssetManager::PathCheck(path, name);
		}

		// Realoding 2D Textures
		AssetManager::Reload2DTextures(registry, assetMap);

		//  Reloading Audio
		AssetManager::ReloadAudioClips(registry, AudioEngineSComponent::Get()->Engine);

		// Reloading 2D Animations
		AssetManager::Reload2DAnimations(registry);

		// Reloading Canvas
		AssetManager::ReloadCanvases(registry);

		// Reloading Scripts
		ScriptingSystem::ReloadScripts(registry, activeScene.m_SceneData.m_ActorPool);
	}

	void WorldAdmin::UpdateEditorCamera(const glm::vec2& gameViewSize, const glm::vec2& sceneViewSize)
	{
		// Rendering scene to the target framebuffer
		RenderScene(m_EditorCamera->GetCamera()->GetViewProjectionMatrix(), m_EditorCamera->m_FrameBuffer, true);
	}

	void WorldAdmin::OnSceneViewResize(float width, float height)
	{
		m_EditorCamera->m_FrameBuffer->OnResize(width, height);
		m_EditorCamera->OnResize(width, height);
	}

	void WorldAdmin::OnGameViewResize(float width, float height)
	{
		for (const auto pair : FramebufferSComponent::Get())
		{
			const auto& [key, framebuffer] = pair;
			framebuffer->OnResize(width, height);
		}

		CameraSystem::OnResize(GetActiveScene().m_SceneData.m_Registry, width, height);
	}

	bool WorldAdmin::Save(const std::string& filePath)
	{
		return GetActiveScene().Save(filePath);
	}

	bool WorldAdmin::Load(const std::string& filePath)
	{
		std::ifstream file(filePath);
		if (!file)
		{
			NATIVE_ERROR("Could not open the file: {}", filePath);
			return false;
		}

		m_SceneMap.clear();
		m_ActiveSceneID = 0;
		CreateScene(filePath);

		if (GetActiveScene().Load(filePath))
		{
			// Reloading Assets
			ReloadAssets();
			CONSOLE_WARN(std::string("Scene loaded successfully"));
			return true;
		}

		return false;
	}

	bool WorldAdmin::SaveCurrentScene()
	{
		SceneData& data = GetActiveScene().GetSceneData();
		// Searching for a file in assets folders if absolute path is not valid and replace old path if file found
		if (AssetManager::PathCheck(data.m_filePath, data.m_fileName))
		{
			return Save(data.m_filePath);
		}

		return false;
	}

	void WorldAdmin::CreateScene(const std::string& filePath)
	{
		Scene newScene(filePath);

		m_ActiveSceneID++;
		m_SceneMap[m_ActiveSceneID] = std::move(newScene);
	}

	bool WorldAdmin::LoadSceneRuntime(uint32_t index)
	{
		auto ref = ProjectConfigSComponent::Get();
		if (ref != nullptr)
		{
			auto result = ref->m_Scenes.find(index);
			if (result == ref->m_Scenes.end())
			{
				NATIVE_ERROR("Could not load the scene, index: {}", index);
				return false;
			}

			auto path = result->second.FilePath;
			auto name = result->second.FileName;

			if (AssetManager::PathCheck(path, name))
			{
				if (Load(path))
				{
					OnPlay();
					return true;
				}
			}
		}

		return false;
	}

	Scene& WorldAdmin::GetActiveScene()
	{
		assert(m_SceneMap.size() > 0);
		assert(m_ActiveSceneID > 0);

		return m_SceneMap[m_ActiveSceneID];
	}

	bool WorldAdmin::LoadProjectConfig()
	{
		ProjectConfigSComponent* ref = ProjectConfigSComponent::Get();
		if (ref == nullptr)
		{
			return false;
		}

		std::string path = "../Config/ProjectConfig.smolconfig";
		if (!AssetManager::PathCheck(path, { "ProjectConfig.smolconfig" }))
		{
			return false;
		}

		std::ifstream file(path);
		std::stringstream storage;
		if (!file)
		{
			NATIVE_ERROR("Could not open the file: {}", path);
			return false;
		}

		storage << file.rdbuf();
		file.close();
		{
			cereal::JSONInputArchive dataInput{ storage };
			dataInput(ref->m_Scenes, ref->m_AssetFolder);
		}
		return true;
	}
}