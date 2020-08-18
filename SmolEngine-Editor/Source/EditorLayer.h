#pragma once
#include "SmolEngineCore.h"
#include "Core/Layer.h"

#include "Core/ECS/Components.h"
#include "Core/ECS/Scene.h"

namespace SmolEngine
{
	class EditorLayer: public Layer
	{
	public:
		EditorLayer()
			:Layer("EditorLayer")
		{

		}

		~EditorLayer() {}

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(DeltaTime deltaTime) override;
		void OnEvent(Event& event) override;
		void OnImGuiRender() override;


	private:

		bool switchState = false;
		float f1 = 0, f2 = -0.8f;

		glm::vec2 m_ViewPortSize = {0.0f, 0.0f};

		Ref<Texture2D> m_Texture;
		Ref<Texture2D> m_SheetTexture;

		Ref<SubTexture2D> m_HouseSubTexture;
		Ref<SubTexture2D> m_FieldSubTexture;

		Scene m_Scene;
		Ref<Actor> m_Actor;

		std::unordered_map<char, SmolEngine::Ref<SmolEngine::SubTexture2D>> m_TextureMap;
	};
}

