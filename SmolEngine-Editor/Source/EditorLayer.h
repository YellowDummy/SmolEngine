#pragma once
#include "SmolEngineCore.h"
#include "Core/Layer.h"
#include "Core/ImGui/EditorConsole.h"
#include "Core/ECS/Components.h"
#include "Core/ECS/Scene.h"
#include "Core/ECS/DefaultSystems.h"

#include <imgui/imgui.h>
#include "Core/ImGui/FileBrowser/imfilebrowser.h"

namespace SmolEngine
{

	enum class ComponentItem : char
	{
		None = 0, Tetxure2D, JinxScript, Camera, AnimationContoller, ParticleSystem
	};

	enum class FileBrowserFlags : uint32_t
	{
		None = 0, ScriptPath, Texture2dPath
	};

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
		void ResetFileBrowser();

	private:
		std::unordered_map<char, SmolEngine::Ref<SmolEngine::SubTexture2D>> m_TextureMap;
		Scene m_Scene;
		glm::vec2 m_ViewPortSize = {0.0f, 0.0f};
		std::string m_FilePath = "";

		Ref<Actor> m_Actor;
		Ref<Actor> m_CameraActor;
		Ref<Actor> m_SelectedActor = nullptr;
		Ref<ImGui::FileBrowser> m_FileBrowser = nullptr;
		Ref<EditorConsole> m_Console = nullptr;
		Ref<Texture2D> m_Texture;
		Ref<Texture2D> m_SheetTexture;
		Ref<SubTexture2D> m_HouseSubTexture;
		Ref<SubTexture2D> m_FieldSubTexture;

		uint32_t m_FileBrowserState = 0;
		bool PlayMode = false;
	};
}

