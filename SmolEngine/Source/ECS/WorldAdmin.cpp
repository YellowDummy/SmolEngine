#include "stdafx.h"
#include "WorldAdmin.h"

#include "Renderer/Framebuffer.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/Renderer.h"
#include "Renderer/EditorCamera.h"
#include "Renderer/Text.h"
#include "Renderer/GraphicsContext.h"
#include "Renderer/TexturesPool.h"

#include "Events/MouseEvent.h"
#include "Events/ApplicationEvent.h"

#include "Core/AssetManager.h"
#include "ImGui/EditorConsole.h"
#include "UI/UIButton.h"
#include "Physics2D/Box2D/CollisionListener2D.h"
#include "Scripting/BehaviourPrimitive.h"

#include "Audio/AudioEngine.h"
#include "Animation/AnimationClip2D.h"

#include "ECS/ComponentsCore.h"
#include "ECS/Systems/RendererSystem.h"
#include "ECS/Systems/Physics2DSystem.h"
#include "ECS/Systems/AudioSystem.h"
#include "ECS/Systems/Animation2DSystem.h"
#include "ECS/Systems/CameraSystem.h"
#include "ECS/Systems/CommandSystem.h"
#include "ECS/Systems/UISystem.h"
#include "ECS/Systems/ScriptingSystem.h"
#include "ECS/Components/Singletons/AudioEngineSComponent.h"
#include "ECS/Components/Singletons/Box2DWorldSComponent.h"
#include "ECS/Components/Singletons/ProjectConfigSComponent.h"
#include "ECS/Components/Singletons/JobsSystemStateSComponent.h"
#include "ECS/Components/Singletons/ScriptingSystemStateSComponent.h"

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <box2d/b2_world.h>

namespace SmolEngine
{
	WorldAdmin::WorldAdmin()
	{
		s_World = this;
	}

	void WorldAdmin::Init()
	{
		LoadStaticComponents();
		m_InPlayMode = false;
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
		if (!AssetManager::PathCheck(sceneData.m_filePath, sceneData.m_Name))
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
					Physics2DSystem::CreateBody(&body, &tranform, &world->World, GetActiveScene().FindActorByID(body.ActorID));
			});
		}

		// Finding which animation / audio clip should play on awake
		AudioSystem::OnAwake(sceneData.m_Registry, &AudioEngineSComponent::Get()->Engine);
		Animation2DSystem::OnAwake(sceneData.m_Registry);

		// Sending start callback to all enabled scripts
		ScriptingSystem::OnBegin(sceneData.m_Registry);
		m_InPlayMode = true;
	}

	void WorldAdmin::OnEndPlay()
	{
		m_InPlayMode = false;
		entt::registry& registry = GetActiveScene().m_SceneData.m_Registry;
		ScriptingSystem::OnEnd(registry);

		// Deleting all Rigidbodies
		Physics2DSystem::DeleteBodies(registry, &Box2DWorldSComponent::Get()->World);

		// Resetting Animation / Audio clips
		AudioSystem::OnReset(registry, &AudioEngineSComponent::Get()->Engine);
		Animation2DSystem::OnReset(registry);
		AudioEngineSComponent::Get()->Engine.Reset();

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
			ScriptingSystem::OnTick(registry, deltaTime);
		}
#else
		Box2DPhysicsSystem::OnUpdate(deltaTime, 6, 2, Box2DWorldSComponent::Get());
		// Sending OnProcess callback
		ScriptingSystem::OnSceneTick(registry, deltaTime);
#endif

		// Extracting Camera
		const auto& cameraGroup = registry.view<CameraComponent, TransformComponent>();
		for (const auto& entity : cameraGroup)
		{
			const auto& [camera, transform] = cameraGroup.get<CameraComponent, TransformComponent>(entity);

			// There is no need to render the scene if the camera is not our target or is disabled
			if (!camera.isPrimaryCamera || !camera.isEnabled) { continue; }

			// Calculating ViewProj
			CameraSystem::CalculateView(&camera, &transform);

			// Rendering scene to target framebuffer
			//RenderScene(tuple.Camera.ViewProjectionMatrix, FramebufferSComponent::Get()[0], false, &tuple.Camera, &transform);

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

			const auto& view = m_SceneData.m_Registry.view<CameraComponent>();

			view.each([&](CameraComponent& camera)
				{
					CameraSystem::OnEvent(camera, e);
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

	void WorldAdmin::RenderScene(const glm::mat4& view, const glm::mat4& proj, const glm::vec3 camPos, float zNear, float zFar, bool debugDrawEnabled,
		CameraComponent* targetCamera, TransformComponent* cameraTranform)
	{
		entt::registry& registry = GetActiveScene().m_SceneData.m_Registry;
#ifdef SMOLENGINE_EDITOR
		if (m_InPlayMode)
			Physics2DSystem::UpdateTransforms(registry);
#else
		Box2DPhysicsSystem::UpdateTransfroms(registry);

#endif
		RendererSystem::BeginDraw(view, proj, camPos, zNear, zFar);
		{
			// 3D
			RendererSystem::SubmitMeshes(registry);
			// 2D
			RendererSystem::Submit2DTextures(registry);
			// Animations
			Animation2DSystem::Update(registry);
			RendererSystem::Submit2DAnimations(registry);
			// Lights
			RendererSystem::SubmitLights(registry);
			//UI
			if (targetCamera != nullptr && cameraTranform != nullptr)
				RendererSystem::SubmitCanvases(registry, targetCamera, cameraTranform);
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
		AssetManager::ReloadAudioClips(registry, &AudioEngineSComponent::Get()->Engine);

		// Reloading 2D Animations
		AssetManager::Reload2DAnimations(registry);

		// Reloading Canvas
		AssetManager::ReloadCanvases(registry);

		// Reloading Materials
		AssetManager::ReloadMeshMaterials(registry, &activeScene.m_SceneData);

		// Reloading Scripts
		ScriptingSystem::ReloadScripts(registry);
	}

	void WorldAdmin::UpdateEditorCamera(Ref<EditorCamera>& cam)
	{
		// Rendering scene to the target framebuffer
		RenderScene(cam->GetViewMatrix(),
			cam->GetProjection(), cam->GetPosition(), cam->GetNearClip(), cam->GetFarClip(), true);
	}

	void WorldAdmin::OnGameViewResize(float width, float height)
	{
		CameraSystem::OnResize(GetActiveScene().m_SceneData.m_Registry, width, height);
	}

	bool WorldAdmin::Save(const std::string& filePath)
	{
		return GetActiveScene().Save(filePath);
	}

	bool WorldAdmin::Load(const std::string& filePath)
	{
		std::string path = filePath;
		std::ifstream file(path);
		if (!file)
		{
			NATIVE_ERROR("Could not open the file: {}", path);
			return false;
		}

		m_SceneMap.clear(); // temp
		m_ActiveSceneID = 0;
		CreateScene(path);

		if (GetActiveScene().Load(path))
		{
			// Reset Pools
			TexturesPool::Reset();
			// Reloading Assets
			ReloadAssets();
			CONSOLE_WARN(std::string("Scene loaded successfully"));
			return true;
		}

		return false;
	}

	bool WorldAdmin::SaveCurrentScene()
	{
		if (m_SceneMap.size() == 0)
			return false;

		SceneData& data = GetActiveScene().GetSceneData();
		// Searching for a file in assets folders if absolute path is not valid and replace old path if file found
		if (AssetManager::PathCheck(data.m_filePath, data.m_Name))
		{
			return Save(data.m_filePath);
		}

		return false;
	}

	void WorldAdmin::CreateScene(const std::string& filePath)
	{
		m_ActiveSceneID++;
		m_SceneMap[m_ActiveSceneID].Init(filePath);
		ReloadAssets();
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

	bool WorldAdmin::LoadStaticComponents()
	{
		auto id = m_GlobalRegistry.create();

		// Engines
		m_GlobalRegistry.emplace<AudioEngineSComponent>(id);
		m_GlobalRegistry.emplace<Box2DWorldSComponent>(id);
		// System States
		m_GlobalRegistry.emplace<ProjectConfigSComponent>(id);
		m_GlobalRegistry.emplace<ScriptingSystemStateSComponent>(id);
		m_GlobalRegistry.emplace<JobsSystemStateSComponent>(id);

		return true;
	}
}