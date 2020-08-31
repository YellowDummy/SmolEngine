#pragma once
#include "SmolEngineCore.h"
#include "Core/Layer.h"
#include "Core/ImGui/EditorConsole.h"
#include "Core/ImGui/CustomWindows.h"
#include "Core/ECS/Components.h"
#include "Core/ECS/Scene.h"
#include "Core/ECS/DefaultSystems.h"

#include <imgui/imgui.h>
#include "Core/ImGui/FileBrowser/imfilebrowser.h"

namespace SmolEngine
{

	enum class ComponentItem : char
	{
		None = 0, 
		Tetxure2D, JinxScript, Rigidbody2D, CameraController, AnimationContoller, ParticleSystem
	};


	enum class FileBrowserFlags : uint32_t
	{
		None = 0, 
		ScriptPath, Texture2dPath, SceneLoad, SceneCreate, SceneSave
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

		glm::vec2 m_ViewPortSize = {0.0f, 0.0f};
		glm::vec2 m_GameViewPortSize = { 0.0f, 0.0f };

		std::string m_FilePath = "";
		Ref<Scene> m_Scene;

		//TEMP 
		Ref<Actor> m_Actor = nullptr;
		Ref<Actor> m_CameraActor = nullptr;
		Ref<Actor> m_SelectedActor = nullptr;

		std::shared_ptr<EditorConsole> m_EditorConsole = nullptr;

		std::unique_ptr<ActorCreationWindow> m_ActorCreationWindow = nullptr;
		std::unique_ptr<ImGui::FileBrowser> m_FileBrowser = nullptr;
		std::unique_ptr<SettingsWindow> m_SettingsWindow = nullptr;

		Ref<Texture2D> m_Texture = nullptr;
		Ref<Texture2D> m_SheetTexture = nullptr;
		Ref<SubTexture2D> m_HouseSubTexture = nullptr;
		Ref<SubTexture2D> m_FieldSubTexture = nullptr;

		FileBrowserFlags m_FileBrowserState = FileBrowserFlags::None;

		bool isSceneViewFocused = false;
		bool isGameViewFocused = false;
	};
}

