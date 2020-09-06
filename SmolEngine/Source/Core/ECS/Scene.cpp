#include "stdafx.h"
#include "Scene.h"

#include <glm/glm.hpp>
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>

#include "Core/ImGui/EditorConsole.h"
#include <glad\glad.h>

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
			rbGroup.get<Rigidbody2DComponent>(body).Rigidbody->CreateBody(m_World);
		}

		auto scriptGroup = s_SceneData.m_Registry.view<ScriptObject>();
		for (auto script : scriptGroup)
		{
			scriptGroup.get<ScriptObject>(script).Start();
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

		auto scriptGroup = s_SceneData.m_Registry.view<ScriptObject>();
		for (auto script : scriptGroup)
		{
			scriptGroup.get<ScriptObject>(script).OnDestroy();
		}

		Load(s_SceneData.m_filePath);
	}

	void Scene::OnUpdate(DeltaTime deltaTime)
	{
		UpdateEditorCamera();

		//Updating all the user's cameras in the current scene
		{
			auto cameraGroup = s_SceneData.m_Registry.group<TransformComponent>(entt::get<CameraComponent>);
			for (auto obj : cameraGroup)
			{
				auto& [cameraTransformComponent, cameraComponent] = cameraGroup.get<TransformComponent, CameraComponent>(obj);
				{
					cameraComponent.Camera->m_FrameBuffer->Bind();

					RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1, 1 });
					RendererCommand::Clear();

					Renderer2D::BeginScene(cameraComponent.Camera->GetCamera(), s_SceneData.m_AmbientStrength);

					auto& group = s_SceneData.m_Registry.group<Texture2DComponent>(entt::get<TransformComponent>);
					for (auto entity : group)
					{
						//Rendering all textures in the current scene
						auto& [transform, texture] = group.get<TransformComponent, Texture2DComponent>(entity);
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
		auto transformView = s_SceneData.m_Registry.view<TransformComponent>();
		for (auto component : transformView)
		{
			auto& obj = transformView.get<TransformComponent>(component);
			if (obj.B2Data != nullptr)
			{
				obj.WorldPos.x = obj.B2Data->B2Pos->p.x;
				obj.WorldPos.y = obj.B2Data->B2Pos->p.y;
				obj.Rotation = *obj.B2Data->B2Rotation;
			}
		}

		//Updating all scripts & rigidbodies in the current scene
		{

			auto scriptGroup = s_SceneData.m_Registry.view<ScriptObject>();
			for (auto script : scriptGroup)
			{
				auto& scriptComponent = scriptGroup.get<ScriptObject>(script);
				if (scriptComponent.Enabled)
				{
					scriptComponent.OnUpdate(deltaTime);
				}
			}

			m_World->Step(deltaTime, 6, 2);
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

		RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RendererCommand::Clear();

		Renderer2D::BeginScene(m_EditorCamera->GetCamera(), s_SceneData.m_AmbientStrength);
		{
			auto& group = s_SceneData.m_Registry.group<Texture2DComponent>(entt::get<TransformComponent>);
			for (auto entity : group)
			{
				auto& [transform, texture] = group.get<TransformComponent, Texture2DComponent>(entity);
				if (texture.Enabled && texture.Texture != nullptr)
				{

					if (transform.B2Data != nullptr)
					{
						glm::vec3 vec3 = glm::vec3(transform.B2Data->B2Pos->p.x, transform.B2Data->B2Pos->p.y, transform.WorldPos.z);
						Renderer2D::DrawSprite(vec3, transform.Scale, *transform.B2Data->B2Rotation, texture.Texture, 1.0f, texture.Color);
					}
					else
					{
						Renderer2D::DrawSprite(transform.WorldPos, transform.Scale, transform.Rotation, texture.Texture, 1.0f, texture.Color);
					}
				}
			}

		}
		Renderer2D::EndScene();

		Renderer2D::BeginDebug(m_EditorCamera->GetCamera());
		{
			auto& group = s_SceneData.m_Registry.group<Rigidbody2DComponent>(entt::get<TransformComponent>);
			for (auto entity : group)
			{
				auto& [transform, rb] = group.get<TransformComponent, Rigidbody2DComponent>(entity);
				
				if (!rb.ShowShape) { continue; }

				if (rb.Rigidbody->m_ShapeType == (int)ShapeType::Box)
				{
					Renderer2D::DebugDraw(DebugPrimitives::Quad, { transform.WorldPos.x, transform.WorldPos.y, 1.0f }, { rb.Rigidbody->m_Shape.x * 2 , rb.Rigidbody->m_Shape.y * 2 }, transform.Rotation);
				}
				else if (rb.Rigidbody->m_ShapeType == (int)ShapeType::Cirlce)
				{
					Renderer2D::DebugDraw(DebugPrimitives::Circle, { transform.WorldPos.x + rb.Rigidbody->m_Offset.x,
transform.WorldPos.y + rb.Rigidbody->m_Offset.y, 1.0f }, { rb.Rigidbody->m_Radius,  rb.Rigidbody->m_Radius }, transform.Rotation);

				}
			}
		}
		Renderer2D::EndDebug();

		{
			auto& group = s_SceneData.m_Registry.group<Light2DComponent>(entt::get<TransformComponent>);
			for (auto entity : group)
			{
				auto& [transform, light] = group.get<TransformComponent, Light2DComponent>(entity);

				Renderer2D::DrawLight2D(transform.WorldPos, light.Light->m_Shape, light.Light->m_Color, light.Light->intensity, m_EditorCamera->GetCamera());
			}
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
		auto cameraGroup = s_SceneData.m_Registry.view<CameraComponent>();
		for (auto& obj : cameraGroup)
		{
			auto& cameraComponent = cameraGroup.get<CameraComponent>(obj);
			cameraComponent.Camera->m_FrameBuffer->OnResize(width, height);
			cameraComponent.Camera->OnResize(width, height);
		}
	}

	bool Scene::AttachScript(std::string& keyName, Ref<Actor> actor)
	{
		if (actor->HasComponent<ScriptObject>())
		{
			NATIVE_WARN("Actor <{}> already has script component", actor->GetName()); return false;
		}

		for (auto item : m_ScriptRegistry)
		{
			auto& key = std::get<0>(item);
			if (key == keyName)
			{
				auto& script = std::get<1>(item);
				auto& ref = actor->AddComponent<ScriptObject>(s_SceneData.m_ID, actor->GetID(), keyName, script->Instantiate());
				ref.Script->m_Actor = actor; 
				return true;
			}
		}

		return false;
	}

	Ref<Actor> Scene::CreateActor(const std::string& name, const std::string& tag)
	{
		if (s_SceneData.m_ID == 0)
		{
			NATIVE_ERROR("The scene is not initialized! Use CreateScene() to initialize the scene"); abort();
		}

		size_t id = std::hash<std::string>{}(name);
		auto obj = std::make_shared<Actor>(s_SceneData.m_Registry.create(), s_SceneData.m_Registry, name, tag, id);

		if (!IsActorExist(obj))
		{
			m_IDSet[name] = id;
			obj->AddComponent<TransformComponent>();
			s_SceneData.m_ActorPool.push_back(obj);
			return obj;
		}

		NATIVE_ERROR("Actor was not created!");
		return nullptr;
	}

	//TODO: Runtime Version
	void Scene::DeleteActor(Ref<Actor> actor)
	{
		{
			if (actor->HasComponent<TransformComponent>())
			{
				actor->DeleteComponent<TransformComponent>();
			}
			if (actor->HasComponent<ScriptObject>())
			{
				actor->DeleteComponent<ScriptObject>();
			}
			if (actor->HasComponent<Texture2DComponent>())
			{
				actor->DeleteComponent<Texture2DComponent>();
			}
			if (actor->HasComponent<Light2DComponent>())
			{
				actor->DeleteComponent<Light2DComponent>();
			}
			if (actor->HasComponent<Rigidbody2DComponent>())
			{
				actor->DeleteComponent<Rigidbody2DComponent>();
			}
			if (actor->HasComponent<CameraComponent>())
			{
				actor->DeleteComponent<CameraComponent>();
			}
			if (actor->HasComponent<JinxScriptComponent>())
			{
				actor->DeleteComponent<JinxScriptComponent>();
			}
		}

		s_SceneData.m_ActorPool.erase(std::remove(s_SceneData.m_ActorPool.begin(), s_SceneData.m_ActorPool.end(), actor), s_SceneData.m_ActorPool.end());
		EDITOR_WARN("Actor <{}> successfully deleted", actor->GetName().c_str());
		actor = nullptr;
	}

	//TODO: Copy ScriptObjects
	void Scene::DuplicateActor(Ref<Actor> actor)
	{
		std::string name = std::string(actor->GetName()) + std::string("_Copy_");
		auto newActor = CreateActor(name);

		if (actor->HasComponent<Texture2DComponent>())
		{
			auto& refTexture = actor->GetComponent<Texture2DComponent>();
			auto& texture = newActor->AddComponent<Texture2DComponent>(refTexture.TexturePath, refTexture.Color);
		}
		if (actor->HasComponent<TransformComponent>())
		{
			auto& refTransform = actor->GetComponent<TransformComponent>();
			auto& transform = newActor->GetComponent<TransformComponent>();

			transform = refTransform;
		}
		if (actor->HasComponent<Light2DComponent>())
		{
			auto& light = newActor->AddComponent<Light2DComponent>();
			light.Light = std::make_shared<Light2D>();

			auto& refLight = actor->GetComponent<Light2DComponent>();

			light.Light->m_Color = refLight.Light->m_Color;
			light.Light->m_Shape = refLight.Light->m_Shape;
			light.Light->intensity = refLight.Light->intensity;
		}
		if (actor->HasComponent<Rigidbody2DComponent>())
		{
			auto& rb = newActor->AddComponent<Rigidbody2DComponent>();
			rb.Rigidbody = std::make_shared<Rigidbody2D>();

			auto& refRb = actor->GetComponent<Rigidbody2DComponent>();

			rb.Rigidbody->m_Actor = newActor;
			rb.Rigidbody->m_Type = refRb.Rigidbody->m_Type;
			rb.Rigidbody->m_canSleep = refRb.Rigidbody->m_canSleep;
			rb.Rigidbody->m_GravityScale = refRb.Rigidbody->m_GravityScale;
			rb.Rigidbody->m_Density = refRb.Rigidbody->m_Density;
			rb.Rigidbody->m_IsAwake = refRb.Rigidbody->m_IsAwake;
			rb.Rigidbody->m_Offset = refRb.Rigidbody->m_Offset;
			rb.Rigidbody->m_IsBullet = refRb.Rigidbody->m_IsBullet;
			rb.Rigidbody->m_Radius = refRb.Rigidbody->m_Radius;
			rb.Rigidbody->m_Restitution = refRb.Rigidbody->m_Restitution;
			rb.Rigidbody->m_Shape = refRb.Rigidbody->m_Shape;
			rb.Rigidbody->m_ShapeType = refRb.Rigidbody->m_ShapeType;
			rb.Rigidbody->m_Friction = refRb.Rigidbody->m_Friction;
		}
		if (actor->HasComponent<CameraComponent>())
		{
			auto& cam = newActor->AddComponent<CameraComponent>();
			auto& refCam = actor->GetComponent<CameraComponent>();

			cam.aspectRatio = refCam.aspectRatio;
			cam.isSelected = refCam.isSelected;
			cam.Camera = std::make_shared<CameraController>(cam.aspectRatio);
			cam.Enabled = refCam.Enabled;
		}
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
.component<Rigidbody2DComponent, CameraComponent, TransformComponent, ScriptObject, Texture2DComponent, Light2DComponent>(output);

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
			CONSOLE_WARN(std::string("Scene saved successfully"));
		}
		else
		{
			CONSOLE_ERROR(std::string("Could not write to file!"));
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
			sceneDataInput(data.m_ActorPool, data.Gravity.x, data.Gravity.y, data.m_ID, data.m_filePath, data.m_Name, data.m_AmbientStrength);
		}

		s_SceneData.m_Registry.clear(); 

		{
			cereal::JSONInputArchive regisrtyInput{ regisrtyStorage };
			entt::snapshot_loader{ s_SceneData.m_Registry }.entities(regisrtyInput).
				component<Rigidbody2DComponent, CameraComponent, TransformComponent, ScriptObject, Texture2DComponent, Light2DComponent>(regisrtyInput);
		}

		m_World = new b2World({ data.Gravity.x, data.Gravity.y });

		{
			auto cameraGroup = s_SceneData.m_Registry.view<CameraComponent>();
			for (auto obj : cameraGroup)
			{
				auto& cameraComponent = cameraGroup.get<CameraComponent>(obj);
				float zoomLevel = cameraComponent.Camera->m_ZoomLevel;
				cameraComponent.Camera = std::make_shared<CameraController>((float)Application::GetApplication().GetWindowHeight()
/ (float)Application::GetApplication().GetWindowWidth());

				cameraComponent.Camera->SetZoom(zoomLevel);
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
			auto scriptGroup = s_SceneData.m_Registry.view<ScriptObject>();
			for (auto obj : scriptGroup)
			{
				auto& script = scriptGroup.get<ScriptObject>(obj);

				for (auto item: m_ScriptRegistry)
				{
					auto& key = std::get<0>(item);
					if (key == script.keyName && script.SceneID == data.m_ID)
					{
						script.Script = m_ScriptRegistry[script.keyName]->Instantiate();

						for (auto actor: data.m_ActorPool)
						{
							if (actor->GetID() == script.ActorID)
							{
								script.Script->m_Actor = actor; break;
							}
						}

						break;
					}
				}
			}
		}

		s_SceneData = data;
		CONSOLE_WARN(std::string("Scene loaded successfully"));
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

	void SceneData::operator=(const SceneData& other)
	{
		m_ActorPool = other.m_ActorPool;
		Gravity.x = other.Gravity.x;
		Gravity.y = other.Gravity.y;
		m_ID = other.m_ID;
		m_filePath = other.m_filePath;
		m_Name = other.m_Name;
		m_AmbientStrength = other.m_AmbientStrength;
	}
}