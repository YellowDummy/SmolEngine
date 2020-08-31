#include "stdafx.h"
#include "Scene.h"

#include <glm/glm.hpp>
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>


namespace SmolEngine
{
	Ref<Scene> Scene::s_Scene = std::make_shared<Scene>();

	void Scene::Init()
	{
		m_World = new b2World(b2Vec2{ s_SceneData.Gravity.x, s_SceneData.Gravity.y });
		s_SceneData.m_Registry = entt::registry();

		m_EditorCamera = std::make_shared<EditorCameraController>(Application::GetApplication().GetWindowWidth() / Application::GetApplication().GetWindowHeight());
		m_EditorCamera->SetZoom(4.0f);
		m_InPlayMode = false;

		//----------------------------------JINXSCRIPT-INITIALIZATION---------------------------------//
		Jinx::GlobalParams globalParams;
		globalParams.enableLogging = true;
		globalParams.logBytecode = true;
		globalParams.logSymbols = true;
		globalParams.enableDebugInfo = true;
		globalParams.logFn = [](Jinx::LogLevel level, const char* msg)
		{
			if (level == Jinx::LogLevel::Error)
			{
				NATIVE_ERROR(msg);
				return;
			}

			printf(msg);
		};
		globalParams.allocBlockSize = 1024 * 16;
		globalParams.allocFn = [](size_t size) { return malloc(size); };
		globalParams.reallocFn = [](void* p, size_t size) { return realloc(p, size); };
		globalParams.freeFn = [](void* p) { free(p); };
		Jinx::Initialize(globalParams);
		m_JinxRuntime = Jinx::CreateRuntime();

		//----------------------------------JINXSCRIPT-INITIALIZATION---------------------------------//
	}

	void Scene::ShutDown()
	{
		Jinx::ShutDown();
	}

	void Scene::OnPlay()
	{
		Save(s_SceneData.m_filePath);

		auto rbGroup = s_SceneData.m_Registry.view<Rigidbody2DComponent>();
		for (auto body : rbGroup)
		{
			rbGroup.get<Rigidbody2DComponent>(body).Rigidbody->CreateBody();
		}

		auto scriptGroup = s_SceneData.m_Registry.view<ScriptComponent>();
		for (auto script : scriptGroup)
		{
			scriptGroup.get<ScriptComponent>(script).Start();
		}

		m_InPlayMode = true;
	}

	void Scene::OnEndPlay()
	{
		m_InPlayMode = false;

		auto rbGroup = s_SceneData.m_Registry.view<Rigidbody2DComponent>();
		for (auto body : rbGroup)
		{
			rbGroup.get<Rigidbody2DComponent>(body).Rigidbody->DeleteBody();
		}

		auto scriptGroup = s_SceneData.m_Registry.view<ScriptComponent>();
		for (auto script : scriptGroup)
		{
			scriptGroup.get<ScriptComponent>(script).OnDestroy();
		}

		Load(s_SceneData.m_filePath);
	}

	void Scene::OnUpdate(DeltaTime deltaTime)
	{
		UpdateEditorCamera();

		//Updating all the user's cameras in the current scene
		{
			auto cameraGroup = s_SceneData.m_Registry.group<TransfromComponent>(entt::get<CameraComponent>);
			for (auto obj : cameraGroup)
			{
				auto& [cameraTransformComponent, cameraComponent] = cameraGroup.get<TransfromComponent, CameraComponent>(obj);
				{
					cameraComponent.Camera->m_FrameBuffer->Bind();

					RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1, 1 });
					RendererCommand::Clear();
					Renderer2D::ResetDataStats();

					Renderer2D::BeginScene(cameraComponent.Camera->GetCamera());

					auto& group = s_SceneData.m_Registry.group<Texture2DComponent>(entt::get<TransfromComponent>);
					for (auto entity : group)
					{
						//Rendering all textures in the current scene
						auto& [transform, texture] = group.get<TransfromComponent, Texture2DComponent>(entity);
						if (texture.Enabled && texture.Texture != nullptr)
						{
							if (transform.B2Data == nullptr)
							{
								Renderer2D::DrawSprite(transform.WorldPos, transform.Scale, transform.Rotation, texture.Texture, 1.0f, texture.Color);
							}
							else if(transform.B2Data != nullptr && m_InPlayMode)
							{
								glm::vec3 vec3 = glm::vec3(transform.B2Data->B2Pos->p.x, transform.B2Data->B2Pos->p.y, 1.0f);
								Renderer2D::DrawSprite(vec3, transform.Scale, *transform.B2Data->B2Rotation, texture.Texture, 1.0f, texture.Color);
							}
						}
					}

					Renderer2D::EndScene();

					cameraComponent.Camera->SetZoom(cameraComponent.Camera->m_ZoomLevel);
					cameraComponent.Camera->SetTransform(cameraTransformComponent.WorldPos);
					cameraComponent.Camera->m_FrameBuffer->UnBind();
				}
			}
		}

		if(!m_InPlayMode) { return; }

		//Updating transform of Rigidbody2D
		auto transformView = s_SceneData.m_Registry.view<TransfromComponent>();
		for (auto component : transformView)
		{
			auto& obj = transformView.get<TransfromComponent>(component);
			if (obj.B2Data != nullptr)
			{
				obj.WorldPos.x = obj.B2Data->B2Pos->p.x;
				obj.WorldPos.y = obj.B2Data->B2Pos->p.y;
				obj.Rotation = *obj.B2Data->B2Rotation;
			}
		}

		//Update all scripts and Rigidbodies in the current scene
		{
			m_World->Step(deltaTime, 6, 2);

			auto scriptGroup = s_SceneData.m_Registry.view<ScriptComponent>();
			for (auto script : scriptGroup)
			{
				auto& scriptComponent = scriptGroup.get<ScriptComponent>(script);
				if (scriptComponent.Enabled)
				{
					scriptComponent.OnUpdate(deltaTime);
				}
			}
		}

	}

	void Scene::OnEvent(Event& e)
	{
		auto cameraGroup = s_SceneData.m_Registry.view<CameraComponent>();
		for (auto obj : cameraGroup)
		{
			auto& cameraComponent = cameraGroup.get<CameraComponent>(obj);
			cameraComponent.Camera->OnSceneEvent(e);
		}
	}

	void Scene::UpdateEditorCamera()
	{
		m_EditorCamera->m_FrameBuffer->Bind();
		RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1, 1 });
		RendererCommand::Clear();
		Renderer2D::ResetDataStats();
		Renderer2D::BeginScene(m_EditorCamera->GetCamera());

		auto& group = s_SceneData.m_Registry.group<Texture2DComponent>(entt::get<TransfromComponent>);
		for (auto entity : group)
		{
			//Rendering all textures in the current scene
			auto& [transform, texture] = group.get<TransfromComponent, Texture2DComponent>(entity);
			if (texture.Enabled && texture.Texture != nullptr)
			{
				if (transform.B2Data != nullptr)
				{
					glm::vec3 vec3 = glm::vec3(transform.B2Data->B2Pos->p.x, transform.B2Data->B2Pos->p.y, 1.0f);
					Renderer2D::DrawSprite(vec3, transform.Scale, *transform.B2Data->B2Rotation, texture.Texture, 1.0f, texture.Color);
				}
				else
				{
					Renderer2D::DrawSprite(transform.WorldPos, transform.Scale, transform.Rotation, texture.Texture, 1.0f, texture.Color);
				}
			}
		}

		Renderer2D::EndScene();
		m_EditorCamera->m_FrameBuffer->UnBind();
	}

	void Scene::OnSceneViewResize(float width, float height)
	{
		m_EditorCamera->m_FrameBuffer->OnResize(width, height);
		m_EditorCamera->OnResize(width, height);
	}

	void Scene::OnGameViewResize(float width, float height)
	{
		auto cameraGroup = s_SceneData.m_Registry.view<CameraComponent>();
		for (auto& obj : cameraGroup)
		{
			auto& cameraComponent = cameraGroup.get<CameraComponent>(obj);
			cameraComponent.Camera->m_FrameBuffer->OnResize(width, height);
			cameraComponent.Camera->OnResize(width, height);
		}
	}

	Ref<Actor> Scene::CreateActor(const std::string& name, const std::string& tag)
	{
		if (s_SceneData.m_ID == 0)
		{
			NATIVE_ERROR("The scene is not initialized! Use CreateScene() to initialize the scene"); abort();
		}

		size_t id = std::hash<std::string>{}(name);
		m_IDSet[name] = id;

		auto obj = std::make_shared<Actor>(s_SceneData.m_Registry.create(), s_SceneData.m_Registry, name, tag, id);
		obj->AddComponent<TransfromComponent>();

		if (!IsActorExist(obj))
		{
			s_SceneData.m_ActorPool.push_back(obj);
			return obj;
		}

		return nullptr;
	}

	void Scene::DeleteActor(const Ref<Actor> actor)
	{
		actor->IsDisabled = true;
		if (actor->HasComponent<Texture2DComponent>())
		{
			actor->GetComponent<Texture2DComponent>().Enabled = false;
		}

		if (actor->HasComponent<ScriptComponent>())
		{
			actor->GetComponent<ScriptComponent>().Enabled = false;
		}

		s_SceneData.m_ActorPool.erase(std::remove(s_SceneData.m_ActorPool.begin(), s_SceneData.m_ActorPool.end(), actor), s_SceneData.m_ActorPool.end());
	}

	void Scene::DeleteActorFromScene(const Ref<Actor> actor)
	{
		actor->IsDisabled = true;
		s_SceneData.m_ActorPool.erase(std::remove(s_SceneData.m_ActorPool.begin(), s_SceneData.m_ActorPool.end(), actor), s_SceneData.m_ActorPool.end());
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

	Ref<Actor> Scene::FindActorByName(const std::string& name)
	{
		size_t id = m_IDSet[name];

		for (auto actor : s_SceneData.m_ActorPool)
		{
			if (actor->GetID() == id)
			{
				return actor;
			}
		}

		return nullptr;
	}

	Ref<Actor> Scene::FindActorByTag(const std::string& tag)
	{
		for (auto actor : s_SceneData.m_ActorPool)
		{
			if (actor->GetTag() == tag)
			{
				return actor;
			}
		}

		return nullptr;
	}

	Ref<Actor> Scene::FindActorByID(size_t id)
	{
		for (auto actor : s_SceneData.m_ActorPool)
		{
			if (actor->GetID() == id)
			{
				return actor;
			}
		}

		return nullptr;
	}

	std::vector<Ref<Actor>> Scene::GetActorListByTag(const std::string& tag)
	{
		std::vector<Ref<Actor>> temp;

		for (auto actor: s_SceneData.m_ActorPool)
		{
			if (actor->GetTag() == tag)
			{
				temp.push_back(actor);
			}

		}

		return temp;
	}

	std::vector<Ref<Actor>>& Scene::GetActorPool()
	{
		return s_SceneData.m_ActorPool;
	}

	const Jinx::RuntimePtr Scene::GetJinxRuntime()
	{
		return m_JinxRuntime;
	}

	void Scene::Save(std::string& filePath)
	{
		std::stringstream storageRegistry;
		std::stringstream storageSceneData;

		{
			cereal::JSONOutputArchive output{ storageRegistry };
			entt::snapshot{ s_SceneData.m_Registry }.entities(output)
.component<Rigidbody2DComponent, CameraComponent, TransfromComponent, ScriptComponent, Texture2DComponent>(output);

		}

		{
			cereal::JSONOutputArchive output{ storageSceneData };
			s_SceneData.serialize(output);
		}

		std::stringstream result;
		result << storageRegistry.str() << "|" << storageSceneData.str();

		std::ofstream myfile(filePath);
		if (myfile.is_open())
		{
			myfile << result.str();
			myfile.close();
			NATIVE_WARN("Scene saved successfully");
		}
		else
		{
			NATIVE_ERROR("Could not write to file!");
		}
	}

	void Scene::Load(std::string& filePath)
	{
		std::ifstream file(filePath);
		std::stringstream buffer;

		if (file)
		{
			buffer << file.rdbuf();
			file.close();
		}
		else 
		{ 
			NATIVE_ERROR("Could not open the file: {}", filePath); return; 
		}

		std::string segment;
		std::vector<std::string> seglist;

		while (std::getline(buffer, segment, '|'))
		{
			seglist.push_back(segment);
		}

		std::stringstream regisrtyStorage;
		regisrtyStorage << seglist.front();

		std::stringstream sceneDataStorage;
		sceneDataStorage << seglist.back();

		SceneData data;
		
		{
			cereal::JSONInputArchive sceneDataInput{ sceneDataStorage };
			sceneDataInput(data.m_ActorPool, data.Gravity.x, data.Gravity.y, data.m_ID, data.m_filePath, data.m_Name);
		}

		s_SceneData.m_Registry.clear();

		{
			cereal::JSONInputArchive regisrtyInput{ regisrtyStorage };
			entt::snapshot_loader{ s_SceneData.m_Registry }.entities(regisrtyInput).
				component<Rigidbody2DComponent, CameraComponent, TransfromComponent, ScriptComponent, Texture2DComponent>(regisrtyInput);
		}

		m_World = new b2World({ data.Gravity.x, data.Gravity.y });

		{
			auto cameraGroup = s_SceneData.m_Registry.view<CameraComponent>();
			for (auto obj : cameraGroup)
			{
				auto& cameraComponent = cameraGroup.get<CameraComponent>(obj);
				cameraComponent.Camera = std::make_shared<CameraController>((float)Application::GetApplication().GetWindowHeight() / (float)Application::GetApplication().GetWindowWidth());
			}
		}

		{
			auto textureGroup = s_SceneData.m_Registry.view<Texture2DComponent>();
			for (auto obj : textureGroup)
			{
				auto& texture = textureGroup.get<Texture2DComponent>(obj);
				texture.Texture = Texture2D::Create(texture.TexturePath);
			}
		}

		{
			auto rbGroup = s_SceneData.m_Registry.view<Rigidbody2DComponent>();
			for (auto obj : rbGroup)
			{
				auto& rb = rbGroup.get<Rigidbody2DComponent>(obj);
				rb.Rigidbody->m_World = m_World;
			}
		}

		{
			auto scriptGroup = s_SceneData.m_Registry.view<ScriptComponent>();
			for (auto obj : scriptGroup)
			{
				auto& script = scriptGroup.get<ScriptComponent>(obj);

				for (auto item : m_Scripts)
				{
					if (script.SceneID == data.m_ID && script.ActorID == item.ActorID)
					{
						script.Script = item.Script; break;
					}
				}
			}
		}

		s_SceneData = data;
		NATIVE_WARN("Scene loaded successfully");
	}

	void Scene::CreateScene(std::string& filePath)
	{
		s_SceneData.m_Registry.clear();

		SceneData newScene;
		newScene.m_filePath = filePath;
		newScene.m_ID = std::hash<std::string>{}(filePath);

		s_SceneData = newScene;
		Save(filePath);
	}


	bool Scene::IsActorExist(Ref<Actor> actor)
	{
		for (auto obj : s_SceneData.m_ActorPool)
		{
			if (obj->GetID() == actor->GetID())
			{
				return true;
			}
		}

		return false;
	}

	SceneData& Scene::GetSceneData()
	{
		return s_SceneData;
	}
}