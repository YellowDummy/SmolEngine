#include "stdafx.h"
#include "Scene.h"

#include <glm/glm.hpp>

namespace SmolEngine
{
	Ref<Scene> Scene::s_Scene = std::make_shared<Scene>();
	static Ref<SceneData> s_SceneData;

	void Scene::Init()
	{
		s_SceneData = std::make_shared<SceneData>();
		m_World = new b2World(s_SceneData->m_Gravity);
		m_Registry = entt::registry();

		FramebufferData m_FramebufferData;
		m_FramebufferData.Width = Application::GetApplication().GetWindowWidth();
		m_FramebufferData.Height = Application::GetApplication().GetWindowHeight();
		s_SceneData->m_FrameBuffer = Framebuffer::Create(m_FramebufferData);

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
		//TODO: Serialize Scene Data
		auto rbGroup = m_Registry.view<Rigidbody2DComponent>();
		for (auto body : rbGroup)
		{
			rbGroup.get<Rigidbody2DComponent>(body).Rigidbody->CreateBody();
		}

		auto scriptGroup = m_Registry.view<ScriptComponent>();
		for (auto script : scriptGroup)
		{
			scriptGroup.get<ScriptComponent>(script).Start();
		}

		m_InPlayMode = true;
	}

	void Scene::OnEndPlay()
	{
		m_InPlayMode = false;

		auto rbGroup = m_Registry.view<Rigidbody2DComponent>();
		for (auto body : rbGroup)
		{
			rbGroup.get<Rigidbody2DComponent>(body).Rigidbody->DeleteBody();
		}

		auto scriptGroup = m_Registry.view<ScriptComponent>();
		for (auto script : scriptGroup)
		{
			scriptGroup.get<ScriptComponent>(script).OnDestroy();
		}
	}

	void Scene::OnUpdate(DeltaTime deltaTime)
	{
		//Updating transform of Rigidbody2D
		auto transformView = m_Registry.view<TransfromComponent>();
		for (auto component: transformView)
		{
			auto& obj = transformView.get<TransfromComponent>(component);
			if (obj.B2Data != nullptr)
			{
				obj.WorldPos.x = obj.B2Data->B2Pos->p.x;
				obj.WorldPos.y = obj.B2Data->B2Pos->p.y;
				obj.Rotation = *obj.B2Data->B2Rotation;
			}
		}

		//Updating all cameras in the current scene
		{
			auto cameraGroup = m_Registry.group<TransfromComponent>(entt::get<CameraComponent>);
			for (auto obj : cameraGroup)
			{
				auto& [cameraTransformComponent, cameraComponent] = cameraGroup.get<TransfromComponent, CameraComponent>(obj);

				s_SceneData->m_FrameBuffer->Bind();
				{
					RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1, 1 });
					RendererCommand::Clear();
					Renderer2D::ResetDataStats();
					Renderer2D::BeginScene(cameraComponent.Camera->GetCamera());

					auto& group = m_Registry.group<Texture2DComponent>(entt::get<TransfromComponent>);
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
				}
				s_SceneData->m_FrameBuffer->UnBind();

				cameraComponent.Camera->OnUpdate(deltaTime, cameraTransformComponent.WorldPos);
			}
		}

		//Updating all scripts & rigibbodies in the current scene
		{
			if (!m_InPlayMode) { return; }

			m_World->Step(deltaTime, 6, 2);

			auto scriptGroup = m_Registry.view<ScriptComponent>();
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
		auto cameraGroup = m_Registry.view<CameraComponent>();
		for (auto obj : cameraGroup)
		{
			auto& cameraComponent = cameraGroup.get<CameraComponent>(obj);
			cameraComponent.Camera->OnEvent(e);
		}
	}

	void Scene::OnEditorResize(float width, float height)
	{
		auto cameraGroup = m_Registry.view<CameraComponent>();
		for (auto& obj : cameraGroup)
		{
			auto& cameraComponent = cameraGroup.get<CameraComponent>(obj);
			cameraComponent.Camera->OnResize(width, height);
		}
	}

	Ref<Actor> Scene::CreateActor(const std::string& name, const std::string& tag)
	{
		size_t id = std::hash<std::string>{}(name);
		m_IDSet[name] = id;

		auto obj = std::make_shared<Actor>(m_Registry.create(), m_Registry, name, tag, id);
		obj->AddComponent<TransfromComponent>();

		if (!IsActorExist(obj))
		{
			s_SceneData->m_ActorPool.push_back(obj);
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

		s_SceneData->m_ActorPool.erase(std::remove(s_SceneData->m_ActorPool.begin(), s_SceneData->m_ActorPool.end(), actor), s_SceneData->m_ActorPool.end());
	}

	void Scene::DeleteActorFromScene(const Ref<Actor> actor)
	{
		actor->IsDisabled = true;
		s_SceneData->m_ActorPool.erase(std::remove(s_SceneData->m_ActorPool.begin(), s_SceneData->m_ActorPool.end(), actor), s_SceneData->m_ActorPool.end());
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

	//TEMP
	Ref<Actor> Scene::FindActorByName(const std::string& name)
	{
		size_t id = m_IDSet[name];

		for (auto actor : s_SceneData->m_ActorPool)
		{
			if (actor->GetID() == id)
			{
				return actor;
			}
		}

		return nullptr;
	}

	//TEMP
	Ref<Actor> Scene::FindActorByTag(const std::string& tag)
	{
		for (auto actor : s_SceneData->m_ActorPool)
		{
			if (actor->GetTag() == tag)
			{
				return actor;
			}
		}

		return nullptr;
	}

	//TEMP
	std::vector<Ref<Actor>> Scene::GetActorListByTag(const std::string& tag)
	{
		std::vector<Ref<Actor>> temp;

		for (auto actor: s_SceneData->m_ActorPool)
		{
			if (actor->GetTag() == tag)
			{
				temp.push_back(actor);
			}

		}

		return temp;
	}

	Ref<Framebuffer> Scene::GetFrameBuffer()
	{
		return s_SceneData->m_FrameBuffer;
	}

	std::vector<Ref<Actor>>& Scene::GetActorPool()
	{
		return s_SceneData->m_ActorPool;
	}

	const Jinx::RuntimePtr Scene::GetJinxRuntime()
	{
		return m_JinxRuntime;
	}

	bool Scene::IsActorExist(Ref<Actor> actor)
	{
		for (auto obj : s_SceneData->m_ActorPool)
		{
			if (obj->GetID() == actor->GetID())
			{
				return true;
			}
		}

		return false;
	}
}