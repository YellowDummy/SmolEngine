#include "stdafx.h"
#include "Scene.h"

#include "Core/ImGui/EditorConsole.h"
#include "Core/Events/MouseEvent.h"
#include "Core/UI/UIButton.h"
#include "Core/Renderer/Text.h"
#include "Core/Physics2D/Box2D/CollisionListener2D.h"

#include "Core/Animation/AnimationClip.h"

#include <filesystem>
#include <glm/glm.hpp>
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <Core\Events\ApplicationEvent.h>


namespace SmolEngine
{
	Ref<Scene> Scene::s_Scene = std::make_shared<Scene>();

	void Scene::Init()
	{
		m_SceneData.m_Registry = entt::registry();
		m_SceneData.m_Entity = m_SceneData.m_Registry.create();

		m_InPlayMode = false;

#ifdef SMOLENGINE_EDITOR

		float aspectRatio = (float)Application::GetApplication().GetWindowWidth() / (float)Application::GetApplication().GetWindowHeight();

		m_EditorCamera = std::make_shared<EditorCameraController>(aspectRatio);
		m_EditorCamera->SetZoom(4.0f);

#endif

	}

	void Scene::StartGame()
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

	void Scene::ShutDown()
	{

	}

	void Scene::OnPlay()
	{

#ifdef SMOLENGINE_EDITOR

		if (!PathCheck(m_SceneData.m_filePath, m_SceneData.m_fileName))
		{
			NATIVE_ERROR("Failed to start the game!");
			return;
		}

		// SceneData must be saved before the simulation starts

		Save(m_SceneData.m_filePath);
#endif

		Box2DWorldSComponent* world = Box2DWorldSComponent::Get();
		Box2DPhysicsSystem::OnPlay(world);

		// Creating rigidbodies & binding joints

		{

			const auto& physicsGroup = m_SceneData.m_Registry.view<PhysicsBaseTuple>();
			for (const auto& entity : physicsGroup)
			{
				auto& physicsTuple = physicsGroup.get<PhysicsBaseTuple>(entity);

				Box2DPhysicsSystem::CreateBody(physicsTuple, world->World);
			}
		}

		// Updating UI Layer

		{

			const auto& cameraGroup = m_SceneData.m_Registry.view<CameraBaseTuple>();
			for (const auto& entity : cameraGroup)
			{
				auto& cameraTuple = cameraGroup.get<CameraBaseTuple>(entity);


			}
		}

		// Finding which animation / audio clip should play on awake

		{
			const auto& resourceGroup = m_SceneData.m_Registry.view<ResourceTuple>();
			for (const auto& entity : resourceGroup)
			{
				auto& resourceTuple = resourceGroup.get<ResourceTuple>(entity);

				AudioSystem::OnAwake(resourceTuple.AudioSource, AudioEngineSComponent::Get()->Engine);
				Animation2DSystem::OnAwake(resourceTuple.Animation2D);
			}
		}

		m_InPlayMode = true;
	}

	void Scene::OnEndPlay()
	{
		m_InPlayMode = false;

		// Deleting all Rigidbodies

		{
			auto world = Box2DWorldSComponent::Get()->World;

			const auto& physicsGroup = m_SceneData.m_Registry.view<PhysicsBaseTuple>();
			for (const auto& entity : physicsGroup)
			{
				auto& physicsTuple = physicsGroup.get<PhysicsBaseTuple>(entity);

				Box2DPhysicsSystem::DeleteBody(&physicsTuple.Body, world);
			}
		}

		// Resetting Animation / Audio clips

		{
			const auto& resourceGroup = m_SceneData.m_Registry.view<ResourceTuple>();
			for (const auto& entity : resourceGroup)
			{
				auto& resourceTuple = resourceGroup.get<ResourceTuple>(entity);

				AudioSystem::OnReset(resourceTuple.AudioSource, AudioEngineSComponent::Get()->Engine);
				Animation2DSystem::OnReset(resourceTuple.Animation2D);
			}
		}

		AudioEngineSComponent::Get()->Engine->Reset();

#ifdef SMOLENGINE_EDITOR

		Load(m_SceneData.m_filePath);
#endif
	}

	// TODO: Implement Double Buffering

	void Scene::OnUpdate(DeltaTime deltaTime)
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

		// Binding Framebuffer

		FramebufferSComponent::Get()[0]->Bind(); // 0 is default framebuffer

		// Pre-Rendering Preparations

		RendererCommand::Reset();
		RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1, 1 });
		RendererCommand::Clear();

		// Extracting All Camera Tuples

		const auto& cameraGroup = m_SceneData.m_Registry.view<CameraBaseTuple>();
		for (const auto& entity : cameraGroup)
		{
			auto& cameraTuple = cameraGroup.get<CameraBaseTuple>(entity);

			// There is no need to render the scene if the camera is not our target or is disabled

			if (!cameraTuple.Camera.isPrimaryCamera || !cameraTuple.Camera.isEnabled) { continue; }

			// Calculating MVP

			CameraSystem::CalculateView(cameraTuple);

			// Rendering scene to target framebuffer

			RenderScene(cameraTuple.Camera.ViewProjectionMatrix, &cameraTuple);

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

	void Scene::OnEvent(Event& e)
	{
		if (e.m_EventType == (int)EventType::S_WINDOW_RESIZE)
		{
			ForEachTuple<CameraBaseTuple>([&](CameraBaseTuple& tuple)
			{
				CameraSystem::OnEvent(tuple, e);
			});

#ifndef  SMOLENGINE_EDITOR

			auto& winResize = static_cast<WindowResizeEvent&>(e);

			for (const auto pair : FramebufferSComponent::Get())
			{
				const auto& [key, framebuffer] = pair;

				framebuffer->OnResize(winResize.GetWidth(), winResize.GetHeight());
			}
#endif 

		}

		if (!m_InPlayMode) { return; }

		if (e.m_EventType == (int)EventType::S_MOUSE_PRESS)
		{
			ForEachTuple<CameraBaseTuple>([&](CameraBaseTuple& tuple)
			{
				UISystem::OnMouseClick(tuple.Canvas, e);
			});
		}

		if (e.m_EventType == (int)EventType::S_MOUSE_MOVE)
		{
			ForEachTuple<CameraBaseTuple>([&](CameraBaseTuple& tuple)
			{
				UISystem::OnMouseMove(tuple.Canvas, e);
			});
		}
	}

	void Scene::RenderScene(const glm::mat4& viewProjectionMatrix, CameraBaseTuple* target)
	{
		// Initializing new DrawList

		Renderer2D::BeginScene(viewProjectionMatrix, m_SceneData.m_AmbientStrength);

		{
			// Default Tuple

			const auto& defaultGroup = m_SceneData.m_Registry.view<DefaultBaseTuple>();
			for (const auto& entity : defaultGroup)
			{
				const auto& baseTuple = defaultGroup.get<DefaultBaseTuple>(entity);

				RendererSystem::RenderDefaultTuple(baseTuple);
			}

			// Physics Tuple

			const auto& physicsGroup = m_SceneData.m_Registry.view<PhysicsBaseTuple>();
			for (const auto& entity : physicsGroup)
			{
				auto& physicsTuple = physicsGroup.get<PhysicsBaseTuple>(entity);

				// Setting b2Transfrom to actor's transform

#ifdef SMOLENGINE_EDITOR

				if (m_InPlayMode)
				{
					Box2DPhysicsSystem::SetTransfrom(physicsTuple);
			    }
#else
				Box2DPhysicsSystem::SetTransfrom(physicsTuple);

#endif

				RendererSystem::RenderPhysicsTuple(physicsTuple);
			}

			// Resource Tuple

			{
				auto& defGroup = m_SceneData.m_Registry.group<DefaultBaseTuple>(entt::get<ResourceTuple>);
				for (auto entity : defGroup)
				{
					auto& [baseTuple, resourceTuple] = defGroup.get<DefaultBaseTuple, ResourceTuple>(entity);

					Animation2DSystem::Update(resourceTuple.Animation2D);
					RendererSystem::RenderAnimation2D(resourceTuple.Animation2D, baseTuple.Transform);
				}

				auto& phGroup = m_SceneData.m_Registry.group<PhysicsBaseTuple>(entt::get<ResourceTuple>);
				for (auto entity : phGroup)
				{
					auto& [phTuple, resourceTuple] = phGroup.get<PhysicsBaseTuple, ResourceTuple>(entity);

					Animation2DSystem::Update(resourceTuple.Animation2D);
					RendererSystem::RenderAnimation2D(resourceTuple.Animation2D, phTuple.Transform);
				}
			}

			// Rendering Camera Tuple

			if (target != nullptr)
			{
				RendererSystem::RenderCameraTuple(*target);
			}
		}

		// Initializing DrawCalls

		Renderer2D::EndScene();
	}

	void Scene::ReloadAssets()
	{
		m_IDSet.clear();
		auto& assetMap = m_SceneData.m_AssetMap;

		// Updating AssetMap

		for (auto& pair : assetMap)
		{
			auto& [name, path] = pair;

			PathCheck(path, name);
		}

		// Extracting Default Tuples

		ForEachTuple<DefaultBaseTuple>([&](DefaultBaseTuple& tuple)
		{
			auto search = assetMap.find(tuple.Texture.FileName);
			if (search != assetMap.end())
			{
				tuple.Texture.Texture = Texture2D::Create(search->second);
			}

			// Updating ID Set

			m_IDSet[tuple.Info.Name] = tuple.Info.ID;
		});


		// Extracting Physics Tuples

		ForEachTuple<PhysicsBaseTuple>([&](PhysicsBaseTuple& tuple)
		{
			auto search = assetMap.find(tuple.Texture.FileName);
			if (search != assetMap.end())
			{
				tuple.Texture.Texture = Texture2D::Create(search->second);
			}

			// Updating ID Set

			m_IDSet[tuple.Info.Name] = tuple.Info.ID;
		});

		// Extracting Resource Tuples

		ForEachTuple<ResourceTuple>([&](ResourceTuple& tuple)
		{
			// Relaoding Audio Clips

			AudioSystem::ReloadAllClips(tuple.AudioSource, AudioEngineSComponent::Get()->Engine);

			// Reloading Animation2D Frames

			for (const auto& pair : tuple.Animation2D.m_Clips)
			{
				auto& [key, clip] = pair;

				for (const auto& framePair : clip->m_Frames)
				{
					const auto& [key, frame] = framePair;

					if (!PathCheck(frame->TexturePath, frame->FileName))
					{
						NATIVE_ERROR("Animation2D reload: texture not found, path: {}!", frame->FileName.c_str());
						continue;
					}

					frame->Texture = Texture2D::Create(frame->TexturePath);
				}
			}
		});

		// Extracting Camera Tuples

		ForEachTuple<CameraBaseTuple>([&](CameraBaseTuple& tuple) 
		{
				UISystem::ReloadElements(tuple.Canvas);
				m_IDSet[tuple.Info.Name] = tuple.Info.ID;
		});
	}

	void Scene::ReloadScripts()
	{

	}

	bool Scene::AddAsset(const std::string& fileName, const std::string& filePath)
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

	bool Scene::DeleteAsset(const std::string& fileName)
	{
		if (fileName == "")
		{
			return false;
		}

		return m_SceneData.m_AssetMap.erase(fileName);
	}

	void Scene::UpdateEditorCamera(const glm::vec2& gameViewSize, const glm::vec2& sceneViewSize)
	{
		m_EditorCamera->m_FrameBuffer->Bind();

		// Pre-Rendering Preparations

		RendererCommand::Reset();
		RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1, 1 });
		RendererCommand::Clear();

		// Rendering scene to the target framebuffer

		RenderScene(m_EditorCamera->GetCamera()->GetViewProjectionMatrix());

		// Rendering debug shapes

		{
			Renderer2D::BeginDebug(m_EditorCamera->GetCamera());
			{
				glm::vec2 res;
				if (gameViewSize == glm::vec2(0.0f))
				{
					res = sceneViewSize;
				}
				else
				{
					res = gameViewSize;
				}

				ForEachTuple<PhysicsBaseTuple>([&](PhysicsBaseTuple& tuple)
				{
					RendererSystem::DebugDraw(tuple);
				});

				ForEachTuple<CameraBaseTuple>([&](CameraBaseTuple& tuple)
				{
						Renderer2D::DebugDraw(DebugPrimitives::Quad, { tuple.Transform.WorldPos.x, tuple.Transform.WorldPos.y, 1.0f },
							{ (res.x / 100.f), (res.y / 100.f) },
							0.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
				});

			}
			Renderer2D::EndDebug();
		}

		m_EditorCamera->m_FrameBuffer->UnBind();
	}

	void Scene::OnSceneViewResize(float width, float height)
	{
		m_EditorCamera->m_FrameBuffer->OnResize(width, height);
		m_EditorCamera->OnResize(width, height);
	}

	void Scene::OnGameViewResize(float width, float height)
	{
		for (const auto pair : FramebufferSComponent::Get())
		{
			const auto& [key, framebuffer] = pair;

			framebuffer->OnResize(width, height);
		}

		const auto& cameraGroup = m_SceneData.m_Registry.view<CameraBaseTuple>();

		for (const auto& entity : cameraGroup)
		{
			auto& cameraTuple = cameraGroup.get<CameraBaseTuple>(entity);

			CameraSystem::OnResize(cameraTuple, width, height);
		}
	}

	bool Scene::AttachScript(const std::string& keyName, const Ref<Actor> actor)
	{

		return true;
	}

	Ref<Actor> Scene::CreateActor(const ActorBaseType baseType, const std::string& name, const std::string& tag)
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
		case ActorBaseType::PhysicsBase:
		{
			auto ref = AddTuple<PhysicsBaseTuple>(*actorRef.get());

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

		m_IDSet[name] = id;
		m_SceneData.m_ActorPool[id] = actorRef;

		return actorRef;
	}

	void Scene::DeleteActor(Ref<Actor> actor)
	{
		
	}

	void Scene::DuplicateActor(Ref<Actor> actor)
	{

	}

	void Scene::AddChild(Ref<Actor> parent, Ref<Actor> child)
	{
		parent->GetChilds().push_back(child);
		child->SetParent(parent);
	}

	void Scene::RemoveChild(Ref<Actor> parent, Ref<Actor> child)
	{
		child->SetParent(nullptr);
		parent->GetChilds().erase(std::remove(parent->GetChilds().begin(), parent->GetChilds().end(), child), parent->GetChilds().end());
	}

	Actor* Scene::FindActorByName(const std::string& name)
	{
		auto& result = m_IDSet.find(name);
		if (result == m_IDSet.end())
		{
			return nullptr;
		}

		return FindActorByID(result->second);
	}

	Actor* Scene::FindActorByTag(const std::string& tag)
	{
		return nullptr;
	}

	Actor* Scene::FindActorByID(const uint32_t id)
	{
		auto& result = m_SceneData.m_ActorPool.find(id);

		if (result != m_SceneData.m_ActorPool.end())
		{
			return result->second.get();
		}

		return nullptr;
	}

	const std::unordered_map<std::string, std::string>& Scene::GetAssetMap()
	{
		return m_SceneData.m_AssetMap;
	}

	std::vector<Ref<Actor>> Scene::GetActorListByTag(const std::string& tag)
	{
		std::vector<Ref<Actor>> temp;

		return temp;
	}

	std::unordered_map<size_t, Ref<Actor>>& Scene::GetActorPool()
	{
		return m_SceneData.m_ActorPool;
	}

	std::vector<Ref<Actor>> Scene::GetActorList()
	{
		std::vector<Ref<Actor>> buffer;

		return buffer;
	}

	std::vector<Actor*> Scene::GetSortedActorList()
	{
		std::vector<Actor*> temp;

		for (uint32_t i = 0; i < m_SceneData.m_ActorPool.size(); ++i)
		{
			for (const auto& pair : m_SceneData.m_ActorPool)
			{
				auto [key, actor] = pair;

				if (actor->Index == i)
				{
					temp.push_back(actor.get());
				}
			}
		}

		return temp;
	}

	bool Scene::Save(const std::string& filePath)
	{
		std::stringstream storageRegistry;
		std::stringstream storageSceneData;

		// Serializing all Tuples

		{
			cereal::JSONOutputArchive output{ storageRegistry };
			entt::snapshot{ m_SceneData.m_Registry }.entities(output).component<DefaultBaseTuple, PhysicsBaseTuple,
				CameraBaseTuple, ResourceTuple>(output);
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

	bool Scene::Load(const std::string& filePath)
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

			entt::snapshot_loader{ m_SceneData.m_Registry }.entities(regisrtyInput).component<DefaultBaseTuple, PhysicsBaseTuple,
				CameraBaseTuple, ResourceTuple>(regisrtyInput);
		}

		// Loading Singletons-Components

		LoadSingletons();

		// Reloading Assets

		ReloadAssets();

		// Reloading Scripts

		ReloadScripts();

		CONSOLE_WARN(std::string("Scene loaded successfully"));
		return true;
	}

	bool Scene::SaveCurrentScene()
	{
		// Searching for a file in assets folders if absolute path is not valid and replace old path if file found

		if (PathCheck(m_SceneData.m_filePath, m_SceneData.m_fileName)) 
		{
			return Save(m_SceneData.m_filePath);
		}

		return false;
	}

	void Scene::CreateScene(const std::string& filePath, const std::string& fileName)
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

	bool Scene::LoadSceneRuntime(uint32_t index)
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

			if (PathCheck(path, name))
			{
				if (Load(path))
				{
					OnPlay();
					return true;
				}
			}

			return false;
		}
	}

	// TODO: Assets folder should be defined by the user (via settings window)

	bool Scene::ChangeFilePath(const std::string& fileName, std::string& pathToChange)
	{
		using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

#ifdef SMOLENGINE_EDITOR // Dodgy implementation

		for (const auto& dirEntry : recursive_directory_iterator(std::string("../GameX/")))
		{
			if (dirEntry.path().filename() == fileName)
			{
				pathToChange = dirEntry.path().u8string();
				return true;
			}
		}

		for (const auto& dirEntry : recursive_directory_iterator(std::string("../SmolEngine/Assets/")))
		{
			if (dirEntry.path().filename() == fileName)
			{
				pathToChange = dirEntry.path().u8string();
				return true;
			}
		}

		for (const auto& dirEntry : recursive_directory_iterator(std::string("../SmolEngine-Editor/")))
		{
			if (dirEntry.path().filename() == fileName)
			{
				pathToChange = dirEntry.path().u8string();
				return true;
			}
		}
#else
		for (const auto& dirEntry : recursive_directory_iterator(std::string("C:/Dev/SmolEngine/")))
		{
			if (dirEntry.path().filename() == fileName)
			{
				pathToChange = dirEntry.path().u8string();
				return true;
			}
		}
#endif


		return false;
	}

	bool Scene::IsPathValid(const std::string& path)
	{
		return std::filesystem::exists(path);
	}

	bool Scene::UpdateIDSet(const std::string& lastName, const std::string& newName)
	{
		auto resultNew = m_IDSet.find(newName);
		if (resultNew != m_IDSet.end())
		{
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

	// Return true if path is valid

	bool Scene::PathCheck(std::string& path, const std::string& fileName) 
	{
		if (path.empty() || fileName.empty())
		{
			CONSOLE_ERROR(std::string("Invalid Path!"));
			return false;
		}

		if (!IsPathValid(path))
		{
			if (!ChangeFilePath(fileName, path))
			{
				CONSOLE_ERROR(std::string("Asset ") + fileName + std::string(" not found"));
				return false;
			}
		}

		return true;
	}

	void Scene::LoadSingletons()
	{
		m_SceneData.m_Registry.emplace_or_replace<SingletonTuple>(m_SceneData.m_Entity);
	}

	void Scene::DeleteSingletons()
	{
		m_SceneData.m_Registry.remove_if_exists<SingletonTuple>(m_SceneData.m_Entity);
	}


	bool Scene::LoadProjectConfig()
	{
		ProjectConfigSComponent* ref = ProjectConfigSComponent::Get();
		if (ref == nullptr)
		{
			return false;
		}

		std::string path = "../Config/ProjectConfig.smolconfig";
		if (!PathCheck(path, { "ProjectConfig.smolconfig" }))
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

	SceneData& Scene::GetSceneData()
	{
		return m_SceneData;
	}
}