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
#include "Core/ECS/Systems/Box2DPhysicsSystem.h"
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
	Ref<WorldAdmin> WorldAdmin::s_Scene = std::make_shared<WorldAdmin>();

	void WorldAdmin::Init()
	{
		// Creating entt registry and parent entity

		m_SceneData.m_Registry = entt::registry();
		m_SceneData.m_Entity = m_SceneData.m_Registry.create();
	}

	void WorldAdmin::InitSystems()
	{
		m_InPlayMode = false;

#ifdef SMOLENGINE_EDITOR

		float aspectRatio = (float)Application::GetApplication().GetWindowWidth() / (float)Application::GetApplication().GetWindowHeight();

		m_EditorCamera = std::make_shared<EditorCameraController>(aspectRatio);
		m_EditorCamera->SetZoom(4.0f);

#endif
		// Initializing user's systems
#if 0
		for (const auto& pair: SystemRegistry::Get()->m_SystemMap)
		{
			const auto& [name, type] = pair;

			SystemInstance instance;
			instance.type = rttr::type::get_by_name(name.c_str());
			instance.variant = instance.type.create(); // instance

			m_SystemMap[name] = instance;
		}

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
#ifdef SMOLENGINE_EDITOR
		if (!AssetManager::PathCheck(m_SceneData.m_filePath, m_SceneData.m_fileName))
		{
			NATIVE_ERROR("Failed to start the game!");
			return;
		}
		// We save the current scene before starting the simulation
		Save(m_SceneData.m_filePath);
#endif
		Box2DWorldSComponent* world = Box2DWorldSComponent::Get();

		// Setting Box2D Callbacks
		Box2DPhysicsSystem::OnBegin(world);

		// Creating rigidbodies and joints
		{
			const auto& view = m_SceneData.m_Registry.view<TransformComponent, Body2DComponent>();
			view.each([&](TransformComponent& tranform, Body2DComponent& body)
			{
				Box2DPhysicsSystem::CreateBody(&body, &tranform, world->World, FindActorByID(body.ActorID));
			});
		}

		// Finding which animation / audio clip should play on awake
		AudioSystem::OnAwake(m_SceneData.m_Registry, AudioEngineSComponent::Get()->Engine);
		Animation2DSystem::OnAwake(m_SceneData.m_Registry);

		// Sending start callback to all systems-scripts
		ScriptingSystem::OnSceneBegin(m_SceneData.m_Registry);
		m_InPlayMode = true;
	}

	void WorldAdmin::OnEndPlay()
	{
		m_InPlayMode = false;
		ScriptingSystem::OnSceneEnd(m_SceneData.m_Registry);

		// Deleting all Rigidbodies
		Box2DPhysicsSystem::DeleteBodies(m_SceneData.m_Registry, Box2DWorldSComponent::Get()->World);

		// Resetting Animation / Audio clips
		AudioSystem::OnReset(m_SceneData.m_Registry, AudioEngineSComponent::Get()->Engine);
		Animation2DSystem::OnReset(m_SceneData.m_Registry);
		AudioEngineSComponent::Get()->Engine->Reset();

#ifdef SMOLENGINE_EDITOR
		Load(m_SceneData.m_filePath);
#endif
	}

	void WorldAdmin::OnUpdate(DeltaTime deltaTime)
	{
		// Updating Phycics
#ifdef SMOLENGINE_EDITOR

		if (m_InPlayMode)
		{
			Box2DPhysicsSystem::OnUpdate(deltaTime, 6, 2, Box2DWorldSComponent::Get());
		}
#else
		Box2DPhysicsSystem::OnUpdate(deltaTime, 6, 2, Box2DWorldSComponent::Get());

#endif
		// Sending OnProcess callback
		ScriptingSystem::OnSceneTick(m_SceneData.m_Registry, deltaTime);

		// Binding Framebuffer
		FramebufferSComponent::Get()[0]->Bind(); // 0 is default framebuffer

		// Pre-Rendering Preparations
		RendererCommand::Reset();
		RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1, 1 });
		RendererCommand::Clear();

		// Extracting Camera Tuples
		const auto& cameraGroup = m_SceneData.m_Registry.view<CameraBaseTuple, TransformComponent>();
		for (const auto& entity : cameraGroup)
		{
			auto& [tuple, transform] = cameraGroup.get<CameraBaseTuple, TransformComponent>(entity);

			// There is no need to render the scene if the camera is not our target or is disabled
			if (!tuple.Camera.isPrimaryCamera || !tuple.Camera.isEnabled) { continue; }

			// Calculating MVP
			CameraSystem::CalculateView(&tuple.Camera, &transform);

			// Rendering scene to target framebuffer
			RenderScene(tuple.Camera.ViewProjectionMatrix, FramebufferSComponent::Get()[0], &tuple.Camera, &transform);

			// At the moment we support only one viewport
			break;
		}

		// Unbinding Framebuffer
		FramebufferSComponent::Get()[0]->UnBind(); // 0 is default framebuffer

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
		UISystem::OnEvent(m_SceneData.m_Registry, e);
	}

	void WorldAdmin::RenderScene(const glm::mat4& viewProjectionMatrix, Ref<Framebuffer> framebuffer, CameraComponent* targetCamera, TransformComponent* cameraTranform)
	{
#ifdef SMOLENGINE_EDITOR
		if (m_InPlayMode)
			Box2DPhysicsSystem::UpdateTransforms(m_SceneData.m_Registry);
#else
		Box2DPhysicsSystem::UpdateTransfroms(m_SceneData.m_Registry);

#endif
		RendererSystem::BeginDraw(viewProjectionMatrix, m_SceneData.m_AmbientStrength, framebuffer);
		{
			auto& registry = m_SceneData.m_Registry;

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
	}

	void WorldAdmin::ReloadAssets()
	{
		UpdateIDSet();
		auto& assetMap = m_SceneData.m_AssetMap;
		// Updating AssetMap
		for (auto& pair : assetMap)
		{
			auto& [name, path] = pair;
			AssetManager::PathCheck(path, name);
		}

		// Realoding 2D Textures
		AssetManager::Reload2DTextures(m_SceneData.m_Registry, assetMap);

		//  Reloading Audio
		AssetManager::ReloadAudioClips(m_SceneData.m_Registry, AudioEngineSComponent::Get()->Engine);

		// Reloading 2D Animations
		AssetManager::Reload2DAnimations(m_SceneData.m_Registry);

		// Reloading Canvas
		AssetManager::ReloadCanvases(m_SceneData.m_Registry);

		ScriptingSystem::ReloadScripts(m_SceneData.m_Registry, m_SceneData.m_ActorPool);
	}

	bool WorldAdmin::AddAsset(const std::string& fileName, const std::string& filePath)
	{
		const auto& result = m_SceneData.m_AssetMap.find(fileName);
		if (result == m_SceneData.m_AssetMap.end())
		{
			m_SceneData.m_AssetMap[fileName] = filePath;
			return true;
		}

		NATIVE_WARN("AssetMap: File already exists!");
		return false;
	}

	bool WorldAdmin::DeleteAsset(const std::string& fileName)
	{
		if (fileName == "")
			return false;

		return m_SceneData.m_AssetMap.erase(fileName);
	}

	void WorldAdmin::UpdateEditorCamera(const glm::vec2& gameViewSize, const glm::vec2& sceneViewSize)
	{
		m_EditorCamera->m_FrameBuffer->Bind();

		// Pre-Rendering Preparations
		RendererCommand::Reset();
		RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1, 1 });
		RendererCommand::Clear();
		// Rendering scene to the target framebuffer

		RenderScene(m_EditorCamera->GetCamera()->GetViewProjectionMatrix(), m_EditorCamera->m_FrameBuffer);

		Renderer2D::BeginDebug();
		{
			RendererSystem::DebugDraw(m_SceneData.m_Registry);
		}
		Renderer2D::EndDebug();

		m_EditorCamera->m_FrameBuffer->UnBind();
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

		CameraSystem::OnResize(m_SceneData.m_Registry, width, height);
	}

	Ref<Actor> WorldAdmin::CreateActor(const ActorBaseType baseType, const std::string& name, const std::string& tag)
	{
		if (m_SceneData.m_ID == 0)
		{
			NATIVE_ERROR("The scene is not initialized! Use CreateScene() to initialize the scene");
			abort();
		}

		// Checking if actor already exists
		const auto searchNameResult = m_IDSet.find(name);
		if (searchNameResult != m_IDSet.end())
		{
			NATIVE_ERROR("Actor {} already exist!", name);
			return 0;
		}

		// Generating ID
		auto actorEntity = m_SceneData.m_Registry.create();
		uint32_t id = (uint32_t)actorEntity;

		// Creating Actor
		auto& actorRef = std::make_shared<Actor>(baseType, actorEntity, m_SceneData.m_ActorPool.size());

		switch (baseType)
		{
		case ActorBaseType::DefaultBase:
		{
			auto ref = AddTuple<DefaultBaseTuple>(*actorRef.get());
			ref->Info.ID = id;
			ref->Info.Name = name;
			ref->Info.Tag = tag;

			break;
		}
		case ActorBaseType::CameraBase:
		{
			auto ref = AddTuple<CameraBaseTuple>(*actorRef.get());
			ref->Info.ID = id;
			ref->Info.Name = name;
			ref->Info.Tag = tag;

			break;
		}
		default:
			break;
		}

		AddComponent<TransformComponent>(*actorRef.get());
		m_IDSet[name] = id;
		m_SceneData.m_ActorPool[id] = actorRef;

		return actorRef;
	}

	void WorldAdmin::DeleteActor(Ref<Actor>& actor)
	{
		bool result_id = m_IDSet.erase(actor->GetName());
		bool result_pool = m_SceneData.m_ActorPool.erase(actor->GetID());

		m_SceneData.m_Registry.remove_if_exists<DefaultBaseTuple>(*actor);
		m_SceneData.m_Registry.remove_if_exists<CameraBaseTuple>(*actor);

		m_SceneData.m_Registry.remove_if_exists<Body2DComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<Texture2DComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<TransformComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<BehaviourComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<Animation2DComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<Light2DSourceComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<CanvasComponent>(*actor);
		m_SceneData.m_Registry.remove_if_exists<AudioSourceComponent>(*actor);

		actor = nullptr;
	}

	void WorldAdmin::DuplicateActor(Ref<Actor>& actor)
	{

	}

	void WorldAdmin::AddChild(Ref<Actor>& parent, Ref<Actor>& child)
	{
		parent->GetChilds().push_back(child);
		child->SetParent(parent);
	}

	BehaviourComponent* WorldAdmin::AddBehaviour(const std::string& systemName, const Ref<Actor>& actor)
	{
		auto& systemMap = SystemRegistry::Get()->m_SystemMap;
		const auto& result = systemMap.find(systemName);
		if (result == systemMap.end())
		{
			NATIVE_ERROR("System <{}> not found!", systemName);
			return nullptr;
		}

		BehaviourComponent* behaviour = nullptr;
		{
			if (!HasComponent<BehaviourComponent>(*actor))
			{
				behaviour = AddComponent<BehaviourComponent>(*actor);
				behaviour->Actor = actor;
				behaviour->ID = actor->GetID();
			}
			else
			{
				behaviour = GetComponent<BehaviourComponent>(*actor);
			}
		}

		uint32_t index = actor->m_ComponentsCount;
		actor->m_ComponentsCount++;
		ScriptInstance instance = {};
		{
			instance.type = rttr::type::get_by_name(systemName);
			instance.variant = instance.type.create();
		}

		std::vector< OutValue> tempValues;
		// Creating out-variables 
		{
			auto& primitive = instance.variant.get_wrapped_value_non_const<BehaviourPrimitive>();
			primitive.m_Actor = actor;

			for (const auto& pair : primitive.m_OutFloatVariables)
			{
				const auto [varName, varValue] = pair;

				OutValue value = OutValue(varName, *varValue, OutValueType::Float);
				tempValues.push_back(value);
			}

			for (const auto& pair : primitive.m_OutIntVariables)
			{
				const auto [varName, varValue] = pair;

				OutValue value = OutValue(varName, *varValue, OutValueType::Int);
				tempValues.push_back(value);
			}

			for (const auto& pair : primitive.m_OutStringVariables)
			{
				const auto& [varName, varValue] = pair;

				OutValue value = OutValue(varName, *varValue, OutValueType::String);
				strcpy(value.stringBuffer, varValue->data());
				tempValues.push_back(value);
			}
		}

		behaviour->OutValues[systemName].ScriptID = index;
		behaviour->OutValues[systemName].OutValues = std::move(tempValues);
		behaviour->Scripts.push_back(std::move(instance));
		return behaviour;
	}

	void WorldAdmin::RemoveChild(Ref<Actor>& parent, Ref<Actor>& child)
	{
		child->SetParent(nullptr);
		parent->GetChilds().erase(std::remove(parent->GetChilds().begin(), parent->GetChilds().end(), child), parent->GetChilds().end());
	}

	Ref<Actor> WorldAdmin::FindActorByName(const std::string& name)
	{
		auto& result = m_IDSet.find(name);
		if (result == m_IDSet.end())
		{
			return nullptr;
		}

		return FindActorByID(result->second);
	}

	Ref<Actor> WorldAdmin::FindActorByTag(const std::string& tag)
	{
		return nullptr;
	}

	Ref<Actor> WorldAdmin::FindActorByID(const uint32_t id)
	{
		auto& result = m_SceneData.m_ActorPool.find(id);

		if (result != m_SceneData.m_ActorPool.end())
		{
			return result->second;
		}

		return nullptr;
	}

	const std::unordered_map<std::string, std::string>& WorldAdmin::GetAssetMap()
	{
		return m_SceneData.m_AssetMap;
	}

	std::vector<Ref<Actor>> WorldAdmin::GetActorListByTag(const std::string& tag)
	{
		std::vector<Ref<Actor>> temp;
		return temp;
	}

	std::unordered_map<size_t, Ref<Actor>>& WorldAdmin::GetActorPool()
	{
		return m_SceneData.m_ActorPool;
	}

	std::vector<Ref<Actor>> WorldAdmin::GetActorList()
	{
		std::vector<Ref<Actor>> buffer;

		return buffer;
	}

	std::vector<Ref<Actor>> WorldAdmin::GetSortedActorList()
	{
		std::vector<Ref<Actor>> temp;

		for (uint32_t i = 0; i < m_SceneData.m_ActorPool.size(); ++i)
		{
			for (const auto& pair : m_SceneData.m_ActorPool)
			{
				auto [key, actor] = pair;

				if (actor->m_Index == i)
				{
					temp.push_back(actor);
				}
			}
		}

		return temp;
	}

	bool WorldAdmin::Save(const std::string& filePath)
	{
		std::stringstream storageRegistry;
		std::stringstream storageSceneData;

		// Serializing all Tuples

		{
			cereal::JSONOutputArchive output{ storageRegistry };
			entt::snapshot{ m_SceneData.m_Registry }.entities(output).component<
				
				DefaultBaseTuple, CameraBaseTuple,

				BehaviourComponent, Texture2DComponent, Animation2DComponent, 
				Light2DSourceComponent, AudioSourceComponent, TransformComponent,
				CanvasComponent, Body2DComponent>(output);
		}

		// Serializing scene data

		{
			cereal::JSONOutputArchive output{ storageSceneData };
			m_SceneData.serialize(output);
		}

		// Merging two streams

		std::stringstream result;
		result << storageRegistry.str() << "|" << storageSceneData.str();

		// Writing result to a file

		std::ofstream myfile(filePath);
		if (myfile.is_open())
		{
			myfile << result.str();
			myfile.close();
			CONSOLE_WARN(std::string("Scene saved successfully"));
			return true;
		}

		CONSOLE_ERROR(std::string("Could not write to a file!"));
		return false;
	}

	bool WorldAdmin::Load(const std::string& filePath)
	{
		std::ifstream file(filePath);
		std::stringstream buffer;

		if (!file)
		{
			NATIVE_ERROR("Could not open the file: {}", filePath);
			return false;
		}

		// Copying file content to a buffer

		buffer << file.rdbuf();
		file.close();

		std::string segment;
		std::vector<std::string> seglist;

		// Spliting string into two

		while (std::getline(buffer, segment, '|'))
		{
			seglist.push_back(segment);
		}

		if (seglist.size() != 2)
		{
			return false;
		}

		// Tuples

		std::stringstream regisrtyStorage;
		regisrtyStorage << seglist.front();

		// Scene data

		std::stringstream sceneDataStorage;
		sceneDataStorage << seglist.back();

		// Deserializing scene data to a new scene object

		{
			cereal::JSONInputArchive sceneDataInput{ sceneDataStorage };

			sceneDataInput(m_SceneData.m_ActorPool, 
				m_SceneData.m_AssetMap,
				m_SceneData.m_Entity, m_SceneData.m_Gravity.x,
				m_SceneData.m_Gravity.y, m_SceneData.m_ID,
				m_SceneData.m_filePath, m_SceneData.m_fileName, 
				m_SceneData.m_Name, m_SceneData.m_AmbientStrength);
		}

		// Deleting Singletons-Components

		DeleteSingletons();

		// The registry must be cleared before writing new data

		m_SceneData.m_Registry.clear();

		// Deserializing components data to an existing registry object

		{
			cereal::JSONInputArchive regisrtyInput{ regisrtyStorage };

			entt::snapshot_loader{ m_SceneData.m_Registry }.entities(regisrtyInput).component<

				DefaultBaseTuple, CameraBaseTuple,

				BehaviourComponent, Texture2DComponent, Animation2DComponent,
				Light2DSourceComponent, AudioSourceComponent, TransformComponent,
				CanvasComponent, Body2DComponent>(regisrtyInput);

		}

		// Loading Singletons-Components

		LoadSingletons();

		// Reloading Assets

		ReloadAssets();

		CONSOLE_WARN(std::string("Scene loaded successfully"));
		return true;
	}

	bool WorldAdmin::SaveCurrentScene()
	{
		// Searching for a file in assets folders if absolute path is not valid and replace old path if file found

		if (AssetManager::PathCheck(m_SceneData.m_filePath, m_SceneData.m_fileName))
		{
			return Save(m_SceneData.m_filePath);
		}

		return false;
	}

	void WorldAdmin::CreateScene(const std::string& filePath, const std::string& fileName)
	{
		m_SceneData.m_Registry.clear();

		SceneData newScene;
		newScene.m_filePath = filePath;
		newScene.m_fileName = fileName;
		newScene.m_ID = std::hash<std::string>{}(filePath);

		m_SceneData = newScene;

		m_SceneData.m_Registry = entt::registry();
		m_SceneData.m_Entity = m_SceneData.m_Registry.create();

		LoadSingletons();
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

	bool WorldAdmin::OnActorNameChanged(const std::string& lastName, const std::string& newName)
	{
		auto resultNew = m_IDSet.find(newName);
		if (resultNew != m_IDSet.end())
		{
			CONSOLE_ERROR("Actor with name " + newName + " already exist!");
			return false;
		}

		size_t id = m_IDSet[lastName];
		if (m_IDSet.erase(lastName) == 1)
		{
			m_IDSet[newName] = id;
			return true;
		}
		return false;
	}

	void WorldAdmin::LoadSingletons()
	{
		m_SceneData.m_Registry.emplace_or_replace<SingletonTuple>(m_SceneData.m_Entity);
	}

	void WorldAdmin::UpdateIDSet()
	{
		m_IDSet.clear();
		{
			const auto& view = m_SceneData.m_Registry.view<DefaultBaseTuple>();
			view.each([&](DefaultBaseTuple& tuple)
			{
				m_IDSet[tuple.Info.Name] = tuple.Info.ID;
			});
		}
		{
			const auto& view = m_SceneData.m_Registry.view<CameraBaseTuple>();
			view.each([&](CameraBaseTuple& tuple)
			{
				m_IDSet[tuple.Info.Name] = tuple.Info.ID;
			});
		}
	}

	void WorldAdmin::DeleteSingletons()
	{
		m_SceneData.m_Registry.remove_if_exists<SingletonTuple>(m_SceneData.m_Entity);
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

	SceneData& WorldAdmin::GetSceneData()
	{
		return m_SceneData;
	}
}