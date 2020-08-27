#pragma once

#include <string>
#include <utility>
#include <vector>

#include "Core/ECS/Components.h"
#include "Core/ECS/Actor.h"

#include "Core/Renderer/Framebuffer.h"
#include "Core/Renderer/Renderer2D.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/Camera.h"
#include "Core/EventHandler.h"
#include "Core/Application.h"

#include <Jinx.hpp>

namespace SmolEngine
{
	class Scene
	{
	public:
		Scene()
		{ 
			FramebufferData m_FramebufferData;
			m_FramebufferData.Width = Application::GetApplication().GetWindowWidth();
			m_FramebufferData.Height = Application::GetApplication().GetWindowHeight();
			m_FrameBuffer = Framebuffer::Create(m_FramebufferData);

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

		~Scene() {}

		void OnUpdate(DeltaTime deltaTime)
		{
			//Updating all cameras in the current scene
			{
				auto cameraGroup = m_Registry.group<TransfromComponent>(entt::get<CameraComponent>);
				for (auto obj : cameraGroup)
				{
					auto& [cameraTransformComponent, cameraComponent] = cameraGroup.get<TransfromComponent, CameraComponent>(obj);

					m_FrameBuffer->Bind();
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
								Renderer2D::DrawSprite(transform.WorldPos, transform.Scale, transform.Rotation, texture.Texture, 1.0f, texture.Color);
							}
						}

						Renderer2D::EndScene();
					}
					m_FrameBuffer->UnBind();

					cameraComponent.Camera->OnUpdate(deltaTime, cameraTransformComponent.WorldPos);
				}
			}

			//Updating all scripts in the current scene
			{
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

		void OnEvent(Event& e)
		{
			auto cameraGroup = m_Registry.view<CameraComponent>();
			for (auto obj : cameraGroup)
			{
				auto& cameraComponent = cameraGroup.get<CameraComponent>(obj);
				cameraComponent.Camera->OnEvent(e);
			}
		}

		void OnEditorResize(float width, float height)
		{
			auto cameraGroup = m_Registry.view<CameraComponent>();
			for (auto& obj : cameraGroup)
			{
				auto& cameraComponent = cameraGroup.get<CameraComponent>(obj);
				cameraComponent.Camera->OnResize(width, height);
			}
		}

		Ref<Actor> CreateActor(const std::string& name, const std::string& tag = std::string("Default")) 
		{ 
			auto obj = std::make_shared<Actor>(m_Registry.create(), m_Registry, name, tag); 
			if (!IsActorInPool(obj))
			{
				m_ActorPool.push_back(obj);
			}

			return obj;
		}

		bool DeleteActor(const Ref<Actor> actor)
		{
			if (IsActorInPool(actor))
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

				return true;
			}

			return false;
		}

		bool DeleteActorImmediately(const Ref<Actor> actor)
		{
			if (IsActorInPool(actor))
			{
				actor->IsDisabled = true;
				m_ActorPool.erase(std::remove(m_ActorPool.begin(), m_ActorPool.end(), actor), m_ActorPool.end());
				return true;
			}

			return false;
		}

		Ref<Actor> FindActorByName(const std::string& name)
		{
			for (auto actor: m_ActorPool)
			{
				if (actor->GetName() == name)
				{
					return actor;
				}
			}

			return nullptr;
		}

		Ref<Actor> FindActorByTag(const std::string& tag)
		{
			for (auto actor : m_ActorPool)
			{
				if (actor->GetTag() == tag)
				{
					return actor;
				}
			}

			return nullptr;
		}

		std::vector<Ref<Actor>>& GetActorsByName(std::string& name)
		{

		}

		std::vector<Ref<Actor>>& GetActorsByTag(std::string& tag)
		{

		}

		Ref<Framebuffer> GetFrameBuffer() { return m_FrameBuffer; }

		const std::vector<Ref<Actor>>& GetActorPool() { return m_ActorPool; }

		const Jinx::RuntimePtr GetJinxRuntime() { return m_JinxRuntime; }

	private:

		bool IsActorInPool(Ref<Actor> actor)
		{
			return std::find(m_ActorPool.begin(), m_ActorPool.end(), actor) != m_ActorPool.end();
		}

	private:
		Ref<Framebuffer> m_FrameBuffer;
		entt::registry m_Registry;
		Jinx::RuntimePtr m_JinxRuntime;
		std::vector<Ref<Actor>> m_ActorPool;
	};
}