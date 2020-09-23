#include "stdafx.h"
#include "Scene.h"

#include <glm/glm.hpp>
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>

#include "Core/ImGui/EditorConsole.h"
#include <glad/glad.h>
#include <filesystem>

namespace SmolEngine
{
	Ref<Scene> Scene::s_Scene = std::make_shared<Scene>();

	void Scene::Init()
	{
		m_AudioEngine = AudioEngine::GetAudioEngine();
		m_AudioEngine->Init();

		m_World = new b2World(b2Vec2{ m_SceneData.m_Gravity.x, m_SceneData.m_Gravity.y });
		m_SceneData.m_Registry = entt::registry();

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
		//TODO: Save() should return bool
		//SceneData must be saved before the simulation starts
		Save(m_SceneData.m_filePath);

		auto rbGroup = m_SceneData.m_Registry.view<Rigidbody2DComponent>();
		for (auto body : rbGroup)
		{
			//Creating rigidbodies using user data
			rbGroup.get<Rigidbody2DComponent>(body).Rigidbody->CreateBody(m_World);
		}

		//Finding which animation clip should play on awake
		auto& animGroup = m_SceneData.m_Registry.view<Animation2DControllerComponent>();
		for (auto entity : animGroup)
		{
			auto& ref = animGroup.get<Animation2DControllerComponent>(entity).AnimationController;
			ref->Reset();
			ref->OnAwake();
		}

		//Finding which audio clip should play on awake
		auto& audioGroup = m_SceneData.m_Registry.view<AudioSourceComponent>();
		for (auto entity : audioGroup)
		{
			auto& ref = audioGroup.get<AudioSourceComponent>(entity).AS;
			ref->OnAwake();
		}

		auto scriptGroup = m_SceneData.m_Registry.view<ScriptObject>();
		for (auto script : scriptGroup)
		{
			auto gr = scriptGroup.get<ScriptObject>(script);

			//Handling serialized values (out-values)
			//Values that being modified inside editor
			for (auto actorValues : gr.Script->m_Actor->m_OutValues)
			{
				//Values that must be set as soon as simulation begins
				switch (actorValues->Value.index())
				{

				case (uint32_t)OutValueType::Float:
				{
					for (auto scriptValues : gr.Script->m_OutFloatVariables)
					{
						auto [varName, varValue] = scriptValues;

						if (actorValues->Key == varName)
						{
							*varValue = std::get<float>(actorValues->Value);
							break;
						}
					}

					break;
				}
				case (uint32_t)OutValueType::Int:
				{
					for (auto scriptValues : gr.Script->m_OutIntVariables)
					{
						auto [varName, varValue] = scriptValues;

						if (actorValues->Key == varName)
						{
							*varValue = std::get<int>(actorValues->Value);
							break;
						}
					}

					break;
				}
				case (uint32_t)OutValueType::String:
				{
					for (auto scriptValues : gr.Script->m_OutStringVariables)
					{
						auto& [varName, varValue] = scriptValues;

						if (actorValues->Key == varName)
						{
							*varValue = std::get<std::string>(actorValues->Value);
							break;
						}
					}

					break;
				}
				default:
					break;
				}
			}

			//Sending start callback
			gr.Script->Start();

		}

		m_InPlayMode = true;
	}

	void Scene::OnEndPlay()
	{
		m_InPlayMode = false;

		m_AudioEngine->Reset();

		auto rbGroup = m_SceneData.m_Registry.view<Rigidbody2DComponent>();
		for (auto body : rbGroup)
		{
			rbGroup.get<Rigidbody2DComponent>(body).Rigidbody->DeleteBody();
		}

		auto scriptGroup = m_SceneData.m_Registry.view<ScriptObject>();
		for (auto script : scriptGroup)
		{
			scriptGroup.get<ScriptObject>(script).OnDestroy();
		}

		Load(m_SceneData.m_filePath);
	}

	void Scene::OnUpdate(DeltaTime deltaTime)
	{
		m_AudioEngine->Update();

		//TODO: Implement double buffering

		UpdateEditorCamera();

		//Updating all the user's cameras in the current scene
		{
			auto cameraGroup = m_SceneData.m_Registry.group<TransformComponent>(entt::get<CameraComponent>);
			for (auto obj : cameraGroup)
			{
				auto& [cameraTransformComponent, cameraComponent] = cameraGroup.get<TransformComponent, CameraComponent>(obj);
				{
					cameraComponent.Camera->m_FrameBuffer->Bind();

					RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1, 1 });
					RendererCommand::Clear();

					Renderer2D::BeginScene(cameraComponent.Camera->GetCamera(), m_SceneData.m_AmbientStrength);
					{
						auto& group = m_SceneData.m_Registry.group<Texture2DComponent>(entt::get<TransformComponent>);
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
								else if (transform.B2Data != nullptr && m_InPlayMode)
								{
									glm::vec3 vec3 = glm::vec3(transform.B2Data->B2Pos->p.x, transform.B2Data->B2Pos->p.y, 1.0f);
									Renderer2D::DrawSprite(vec3, transform.Scale, *transform.B2Data->B2Rotation, texture.Texture, 1.0f, texture.Color);
								}
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
		auto transformView = m_SceneData.m_Registry.view<TransformComponent>();
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
			auto scriptGroup = m_SceneData.m_Registry.view<ScriptObject>();
			for (auto script : scriptGroup)
			{
				auto& scriptComponent = scriptGroup.get<ScriptObject>(script);
				if (scriptComponent.Enabled)
				{
					scriptComponent.OnUpdate(deltaTime);
				}
			}

			//Updating Box2D logic
			m_World->Step(deltaTime, 6, 2);
		}

	}

	void Scene::OnEvent(Event& e)
	{
		//Sending events to all cameras in the current scene
		{
			auto cameraGroup = m_SceneData.m_Registry.view<CameraComponent>();
			for (auto obj : cameraGroup)
			{
				auto& cameraComponent = cameraGroup.get<CameraComponent>(obj);
				cameraComponent.Camera->OnSceneEvent(e);
			}
		}
	}

	void Scene::UpdateEditorCamera()
	{
		m_EditorCamera->m_FrameBuffer->Bind();

		//
		{
			RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			RendererCommand::Clear();

			Renderer2D::BeginScene(m_EditorCamera->GetCamera(), m_SceneData.m_AmbientStrength);
			{
				auto& group = m_SceneData.m_Registry.group<Texture2DComponent>(entt::get<TransformComponent>);
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

				auto& animGroup = m_SceneData.m_Registry.group<Animation2DControllerComponent>(entt::get<TransformComponent>);
				for (auto entity : animGroup)
				{
				   auto& [animController, transfrom] = animGroup.get<Animation2DControllerComponent, TransformComponent>(entity);

				   if (animController.AnimationController != nullptr)
				   {
					   if (animController.AnimationController->m_CurrentClip != nullptr)
					   {
						   animController.AnimationController->Update();

						   auto& currentClip = animController.AnimationController->m_CurrentClip;

						   if (transfrom.Scale == glm::vec2(1.0f))
						   {
							   Renderer2D::DrawAnimation2D(transfrom.WorldPos, currentClip->Clip->m_CurrentFrameKey->TextureScale,
								   0, currentClip->Clip->m_CurrentFrameKey->Texture, 1.0f, currentClip->Clip->m_CurrentFrameKey->TextureColor);
						   }
						   else
						   {
							   Renderer2D::DrawAnimation2D(transfrom.WorldPos, currentClip->Clip->m_CurrentFrameKey->TextureScale + transfrom.Scale,
								   0, currentClip->Clip->m_CurrentFrameKey->Texture, 1.0f, currentClip->Clip->m_CurrentFrameKey->TextureColor);
						   }

					   }
				   }
				}

			}

			Renderer2D::EndScene();
		}

		//Rendering debug shapes
		{
			Renderer2D::BeginDebug(m_EditorCamera->GetCamera());
			{
				auto& group = m_SceneData.m_Registry.group<Rigidbody2DComponent>(entt::get<TransformComponent>);
				for (auto entity : group)
				{
					auto& [transform, rb] = group.get<TransformComponent, Rigidbody2DComponent>(entity);

					if (!rb.ShowShape) { continue; }

					if (rb.Rigidbody->m_ShapeType == (int)ShapeType::Box)
					{
						Renderer2D::DebugDraw(DebugPrimitives::Quad, { transform.WorldPos.x, transform.WorldPos.y, 1.0f },
							{ rb.Rigidbody->m_Shape.x * 2 , rb.Rigidbody->m_Shape.y * 2 }, transform.Rotation);
					}
					else if (rb.Rigidbody->m_ShapeType == (int)ShapeType::Cirlce)
					{
						Renderer2D::DebugDraw(DebugPrimitives::Circle, { transform.WorldPos.x + rb.Rigidbody->m_Offset.x,
							transform.WorldPos.y + rb.Rigidbody->m_Offset.y, 1.0f }, { rb.Rigidbody->m_Radius,  rb.Rigidbody->m_Radius }, transform.Rotation);

					}
				}
			}
			Renderer2D::EndDebug();
		}

		//Rendering all lights in the current scene
		{
			auto& group = m_SceneData.m_Registry.group<Light2DComponent>(entt::get<TransformComponent>);
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
		auto cameraGroup = m_SceneData.m_Registry.view<CameraComponent>();
		for (auto& obj : cameraGroup)
		{
			auto& cameraComponent = cameraGroup.get<CameraComponent>(obj);
			cameraComponent.Camera->m_FrameBuffer->OnResize(width, height);
			cameraComponent.Camera->OnResize(width, height);
		}
	}

	bool Scene::AttachScript(const std::string& keyName, const Ref<Actor> actor)
	{
		if (actor->HasComponent<ScriptObject>())
		{
			NATIVE_WARN("Actor <{}> already has script component", actor->GetName()); return false;
		}

		auto result = m_ScriptRegistry.find(keyName);
		if (result == m_ScriptRegistry.end())
		{
			CONSOLE_WARN(std::string("Script not found!"));
			return false;
		}

		auto& strKey = result->first;
		auto& scriptObj = result->second;

		auto& ref = actor->AddComponent<ScriptObject>(m_SceneData.m_ID, actor->GetID(), keyName, scriptObj->Instantiate());
		ref.Script->m_Actor = actor;

		//Creating out-variables inside actor class
		{
			for (auto pair : ref.Script->m_OutFloatVariables)
			{
				auto& [varName, varValue] = pair;
				std::shared_ptr<OutValue> value = std::make_shared<OutValue>(varName, *varValue, OutValueType::Float);
				ref.Script->m_Actor->m_OutValues.push_back(value);
			}

			for (auto pair : ref.Script->m_OutIntVariables)
			{
				auto& [varName, varValue] = pair;
				std::shared_ptr<OutValue> value = std::make_shared<OutValue>(varName, *varValue, OutValueType::Int);
				ref.Script->m_Actor->m_OutValues.push_back(value);
			}

			for (auto pair : ref.Script->m_OutStringVariables)
			{
				auto& [varName, varValue] = pair;
				std::shared_ptr<OutValue> value = std::make_shared<OutValue>(varName, *varValue, OutValueType::String);
				strcpy(value->stringBuffer, varValue->data());

				ref.Script->m_Actor->m_OutValues.push_back(value);
			}
		}

		return true;
	}

	Ref<Actor> Scene::CreateActor(const std::string& name, const std::string& tag)
	{
		if (m_SceneData.m_ID == 0)
		{
			NATIVE_ERROR("The scene is not initialized! Use CreateScene() to initialize the scene"); abort();
		}

		size_t id = std::hash<std::string>{}(name);
		auto result = m_SceneData.m_ActorPool.find(id);
		{
			if (result != m_SceneData.m_ActorPool.end())
			{
				NATIVE_ERROR("Actor {} already exist!", name);
				return nullptr;
			}
		}

		auto actor = std::make_shared<Actor>(m_SceneData.m_Registry.create(), m_SceneData.m_Registry, name, tag, id, m_SceneData.m_ActorPool.size());

		m_IDSet[name] = id;
		actor->AddComponent<TransformComponent>();
		m_SceneData.m_ActorPool[id] = actor;

		return actor;
	}

	void Scene::DeleteActor(Ref<Actor> actor)
	{
		{
			//Components
			if (actor->HasComponent<TransformComponent>())
			{
				actor->DeleteComponent<TransformComponent>();
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
			if (actor->HasComponent<Animation2DControllerComponent>())
			{
				actor->DeleteComponent<Animation2DControllerComponent>();
			}

			//Objects
			if (actor->HasComponent<ScriptObject>())
			{
				actor->DeleteComponent<ScriptObject>();
			}
		}

		m_SceneData.m_ActorPool.erase(actor->ID);

		EDITOR_WARN("Actor <{}> successfully deleted", actor->GetName().c_str());
		actor = nullptr;
	}

	//TODO: Copy ScriptObject && AnimationController components
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
		auto i_result = m_IDSet.find(name);
		if (i_result == m_IDSet.end())
		{
			NATIVE_ERROR(std::string("Actor ID not found!"));
			return nullptr;
		}

		auto result = m_SceneData.m_ActorPool.find(i_result->second);
		if (result == m_SceneData.m_ActorPool.end())
		{
			CONSOLE_ERROR(std::string("Actor not found!"));
			return nullptr;
		}

		return result->second;
	}

	Ref<Actor> Scene::FindActorByTag(const std::string& tag)
	{
		for (auto pair : m_SceneData.m_ActorPool)
		{
			auto& [key, actor] = pair;
			if (actor->GetTag() == tag)
			{
				return actor;
			}
		}

		return nullptr;
	}

	Ref<Actor> Scene::FindActorByID(size_t id)
	{
		auto result = m_SceneData.m_ActorPool.find(id);
		if (result == m_SceneData.m_ActorPool.end())
		{
			CONSOLE_ERROR(std::string("Actor not found!"));
			return nullptr;
		}

		return result->second;
	}

	std::vector<Ref<Actor>> Scene::GetActorListByTag(const std::string& tag)
	{
		std::vector<Ref<Actor>> temp;
		for (auto pair : m_SceneData.m_ActorPool)
		{
			auto& [key, actor] = pair;
			if (actor->GetTag() == tag)
			{
				temp.push_back(actor);
			}
		}

		return temp;
	}

	std::unordered_map<size_t, Ref<Actor>>& Scene::GetActorPool()
	{
		return m_SceneData.m_ActorPool;
	}

	std::vector<Ref<Actor>> Scene::GetActorList()
	{
		std::vector<Ref<Actor>> buffer;

		for (auto pair : Scene::GetScene()->GetActorPool())
		{
			auto& [key, actor] = pair;
			buffer.push_back(actor);
		}

		return buffer;
	}

	std::vector<Ref<Actor>> Scene::GetSortedActorList()
	{
		std::vector<Ref<Actor>> buffer;

		for (int i = 0; i < m_SceneData.m_ActorPool.size(); i++)
		{
			for (auto pair : m_SceneData.m_ActorPool)
			{
				auto& [key, actor] = pair;
				if (actor->Index == i)
				{
					buffer.push_back(actor);
					break;
				}
			}
		}

		return buffer;
	}

	const Jinx::RuntimePtr Scene::GetJinxRuntime()
	{
		return m_JinxRuntime;
	}

	void Scene::Save(std::string& filePath)
	{
		std::stringstream storageRegistry;
		std::stringstream storageSceneData;

		//Serializing all components in the current scene
		{
			cereal::JSONOutputArchive output{ storageRegistry };
			entt::snapshot{ m_SceneData.m_Registry }.entities(output)
				.component<Rigidbody2DComponent, CameraComponent, TransformComponent,
				ScriptObject, Texture2DComponent, Light2DComponent, Animation2DControllerComponent,
				AudioSourceComponent>(output);

		}

		//Serializing scene data
		{
			cereal::JSONOutputArchive output{ storageSceneData };
			m_SceneData.serialize(output);
		}

		//Merging two streams
		std::stringstream result;
		result << storageRegistry.str() << "|" << storageSceneData.str();

		//Writing result to a file
		std::ofstream myfile(filePath);
		if (myfile.is_open())
		{
			myfile << result.str();
			myfile.close();
			CONSOLE_WARN(std::string("Scene saved successfully"));
		}
		else
		{
			CONSOLE_ERROR(std::string("Could not write to a file!"));
		}
	}

	void Scene::Load(std::string& filePath)
	{
		std::ifstream file(filePath);
		std::stringstream buffer;

		//Copying file content to a buffer
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

		//Spliting one string into two
		while (std::getline(buffer, segment, '|'))
		{
			seglist.push_back(segment);
		}

		//Components data
		std::stringstream regisrtyStorage;
		regisrtyStorage << seglist.front();

		//Scene data
		std::stringstream sceneDataStorage;
		sceneDataStorage << seglist.back();

		SceneData data;
		
		//Deserializing scene data to a new scene object
		{
			cereal::JSONInputArchive sceneDataInput{ sceneDataStorage };
			sceneDataInput(data.m_ActorPool, data.m_Gravity.x, data.m_Gravity.y, data.m_ID, data.m_filePath, data.m_fileName, data.m_Name, data.m_AmbientStrength);
		}

		m_SceneData.m_Registry.clear(); //The registry must be cleared before writing new data (!!)

		//Deserializing compoentns data to an existing registry object
		{
			cereal::JSONInputArchive regisrtyInput{ regisrtyStorage };
			entt::snapshot_loader{ m_SceneData.m_Registry }.entities(regisrtyInput).
				component<Rigidbody2DComponent, CameraComponent, TransformComponent,
				ScriptObject, Texture2DComponent, Light2DComponent, Animation2DControllerComponent,
				AudioSourceComponent>(regisrtyInput);
		}

		//Creating new Box2D instance using new data
		m_World = new b2World({ data.m_Gravity.x, data.m_Gravity.y });

		//Creating cameras
		{
			auto cameraGroup = m_SceneData.m_Registry.view<CameraComponent>();
			for (auto obj : cameraGroup)
			{
				auto& cameraComponent = cameraGroup.get<CameraComponent>(obj);
				float zoomLevel = cameraComponent.Camera->m_ZoomLevel;
				cameraComponent.Camera = std::make_shared<CameraController>((float)Application::GetApplication().GetWindowHeight()
					/ (float)Application::GetApplication().GetWindowWidth());

				cameraComponent.Camera->SetZoom(zoomLevel);
			}
		}

		//Creating textures
		{
			auto textureGroup = m_SceneData.m_Registry.view<Texture2DComponent>();
			for (auto obj : textureGroup)
			{
				auto& texture = textureGroup.get<Texture2DComponent>(obj);

				if (PathCheck(texture.TexturePath, texture.FileName)) // Searching for a file in assets folders if absolute path is not valid and replace old path if file found
				{
					texture.Texture = Texture2D::Create(texture.TexturePath);
				}
			}
		}

		//Reloading all textures in animation
		{
			auto animControllerGroup = m_SceneData.m_Registry.view<Animation2DControllerComponent>();
			for (auto obj : animControllerGroup)
			{
				auto& controller = animControllerGroup.get<Animation2DControllerComponent>(obj);
				controller.AnimationController->ReloadTextures();
			}
		}

		//Check whether a path is still valid inside audio source component
		{
			auto& audioGroup = m_SceneData.m_Registry.view<AudioSourceComponent>();
			for (auto entity : audioGroup)
			{
				auto& ref = audioGroup.get<AudioSourceComponent>(entity).AS;
				std::unordered_map <std::string, Ref<AudioClip>> buffer;

				for (auto pair: ref->m_AudioClips)
				{
					auto& [key, clip] = pair;
					if (!PathCheck(clip->FilePath, clip->FileName))
					{
						buffer[key] = clip;
					}
				}

				//Deleting an audio clip if path is invalid
				if (!buffer.empty())
				{
					for (auto pair: buffer)
					{
						auto& [key, clip] = pair;
						ref->m_AudioClips.erase(key);
					}
				}
			}
		}

		//Updating ID map
		{
			m_IDSet.clear();

			for (auto pair: data.m_ActorPool)
			{
				auto& [key, actor] = pair;

				m_IDSet[actor->GetName()] = key;
			}
		}

		//Loading scripts
		{
			auto scriptGroup = m_SceneData.m_Registry.view<ScriptObject>();
			for (auto obj : scriptGroup)
			{
				auto& script = scriptGroup.get<ScriptObject>(obj);

				for (auto item: m_ScriptRegistry)
				{
					auto& [strKey, scriptObj] = item;

					if (strKey == script.keyName && script.SceneID == data.m_ID)
					{
						script.Script = m_ScriptRegistry[script.keyName]->Instantiate(); // Create a new instance of the script

						for (auto pair: data.m_ActorPool) // Search for an actor by ID in the current script
						{
							auto& [key, actor] = pair;

							if (actor->GetID() == script.ActorID)
							{
								script.Script->m_Actor = actor; break; 
							}
						}

						//Setting string buffer to saved value in std::variant
						for (auto val : script.Script->m_Actor->m_OutValues)
						{
							if (val->Value.index() == (uint32_t)OutValueType::String)
							{
								strcpy(val->stringBuffer, std::get<std::string>(val->Value).data());
							}
						}

						break;
					}
				}
			}
		}

		m_SceneData = data;
		CONSOLE_WARN(std::string("Scene loaded successfully"));
	}

	void Scene::SaveCurrentScene()
	{
		if (PathCheck(m_SceneData.m_filePath, m_SceneData.m_fileName))  //Searching for a file in assets folders if absolute path is not valid and replace old path if file found
		{
			Save(m_SceneData.m_filePath);
		}
	}

	void Scene::CreateScene(const std::string& filePath, const std::string& fileName)
	{
		m_SceneData.m_Registry.clear();
		SceneData newScene;
		newScene.m_filePath = filePath;
		newScene.m_fileName = fileName;
		newScene.m_ID = std::hash<std::string>{}(filePath);
		m_SceneData = newScene;
	}

	bool Scene::ChangeFilePath(const std::string& fileName, std::string& pathToChange)
	{
		using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

		//TODO: Assets folder should be defined by the user (via settings window)
		for (const auto& dirEntry : recursive_directory_iterator(std::string("../GameX/")))
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

		return false;
	}

	bool Scene::IsPathValid(const std::string& path)
	{
		return std::filesystem::exists(path);
	}

	bool Scene::PathCheck(std::string& path, const std::string& fileName)
	{
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

	SceneData& Scene::GetSceneData()
	{
		return m_SceneData;
	}

	void SceneData::operator=(const SceneData& other)
	{
		m_ActorPool = other.m_ActorPool;
		m_Gravity.x = other.m_Gravity.x;
		m_Gravity.y = other.m_Gravity.y;
		m_ID = other.m_ID;
		m_filePath = other.m_filePath;
		m_fileName = other.m_fileName;
		m_Name = other.m_Name;
		m_AmbientStrength = other.m_AmbientStrength;
	}
}