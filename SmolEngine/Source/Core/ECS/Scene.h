#pragma once

#include <string>
#include <../Libraries/entt/entt.hpp>

#include "Core/Application.h"
#include "Core/Renderer/Framebuffer.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/Camera.h"
#include "Core/Time.h"
#include "Core/SLog.h"
#include "Core/EventHandler.h"
#include "Core/ECS/Components.h"
#include "Core/Renderer/Renderer2D.h"

namespace SmolEngine
{
	class Actor
	{
	public:

		Actor() = default;
		Actor(const entt::entity& actor, entt::registry& reg,  const std::string& tag)
			:Entity(actor), Tag(tag), Reg(reg)
		{
			AddComponent<TransfromComponent>();
		}

		template<typename T>
		T& AddComponent() { return Reg.emplace<T>(Entity);  }

		template<typename T>
		T& GetComponent() { return Reg.get<T>(Entity); }

		template<typename T>
		bool DeleteComponent() { return Reg.remove_if_exists<T>(Entity); }

	private:
		entt::entity Entity;
		std::string Tag;
		entt::registry& Reg;
	};


	class Scene
	{
	public:
		Scene()
			:m_EditorCamera(std::make_shared<CameraController>( Application::GetApplication().GetWindow().GetWidth() / Application::GetApplication().GetWindow().GetHeight()) )
		{ 
			m_EditorCamera->SetZoom(4.0f);

			FramebufferData m_FramebufferData;
			m_FramebufferData.Width = Application::GetApplication().GetWindow().GetWidth();
			m_FramebufferData.Height = Application::GetApplication().GetWindow().GetHeight();


			m_FrameBuffer = Framebuffer::Create(m_FramebufferData);
		}

		~Scene() {}

		void OnUpdate(DeltaTime deltaTime)
		{
			m_FrameBuffer->Bind();

			RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1, 1 });
			RendererCommand::Clear();
			Renderer2D::ResetDataStats();

			Renderer2D::BeginScene(m_EditorCamera->GetCamera());

			auto group = m_Registry.group<TransfromComponent>(entt::get<Texture2DComponent>);

			for (auto entity : group) {

				auto [transform, texture] = group.get<TransfromComponent, Texture2DComponent>(entity);
				Renderer2D::DrawSprite(transform.WorldPos, transform.Scale, transform.Rotation, texture.Texture);
			}

			Renderer2D::EndScene();

			m_FrameBuffer->UnBind();
			m_EditorCamera->OnUpdate(deltaTime);
		}

		Ref<Actor> CreateActor(const std::string& tag = std::string("Default")) { return std::make_shared<Actor>(m_Registry.create(), m_Registry, tag); }
		Ref<Framebuffer> GetFrameBuffer() { return m_FrameBuffer; }
		Ref<CameraController> GetEditorCamera() { return m_EditorCamera; }

	private:
		Ref<Framebuffer> m_FrameBuffer;
		Ref<CameraController> m_EditorCamera;

		entt::registry m_Registry;
	};
}