#pragma once
#include "SmolEngineCore.h"
#include "Core/Layer.h"

#include "Core/ImGui/EditorConsole.h"
#include "Core/ImGui/EditorPanels.h"
#include "Core/ImGui/BuildPanel.h"

#include "Core/ECS/Components.h"
#include "Core/ECS/Scene.h"
#include "Core/ECS/DefaultSystems.h"

#include "Core/ImGui/AnimationPanel.h"
#include "Core/Animation/Animation2D.h"

#include <imgui/imgui.h>

#include "Core/ImGui/FileBrowser/imfilebrowser.h"

namespace SmolEngine
{
	enum class FileBrowserFlags : uint16_t
	{
		None = 0,

		//Textures

		Load_Texture2D,

		//Animation

		Load_Animation_Clip,
		Load_Animation_Clip_Controller,

		//Audio

		Load_Audio_Clip,

		//Scripts

		Load_Jinx_Script,

		//Scene

		Scene_Load,
		Scene_Create, 
		Scene_Save,
		
		//Canvas

		Canvas_Chanage_Button_Texture,
		Canavas_Create_TextLabel,
		Canavas_TextLabel_Load_Font
	};

	enum class ComponentItem : char
	{
		None = 0, 
		Tetxure2D, JinxScript, Rigidbody2D, CameraController, Light2D, Animation2DContoller, AudioSource, Canvas
	};

	enum class SelectionFlags: uint16_t
	{
		None = 0,
		Inspector, Actions
	};

	class Framebuffer;

	class EditorLayer: public Layer
	{
	public:

		EditorLayer()
			:Layer("EditorLayer") {}

		~EditorLayer() {}

		/// Overloads

		void OnAttach() override;

		void OnDetach() override;

		void OnUpdate(DeltaTime deltaTime) override;

		void OnEvent(Event& event) override;

		void OnImGuiRender() override;

	private:

		void ResetFileBrowser();

	private:

		glm::vec2 m_GameViewSize = { 0.0f, 0.0f };

		glm::vec2 m_SceneViewSize = { 0.0f, 0.0f };

		glm::vec2 m_ViewPortSize = {0.0f, 0.0f};

		glm::vec2 m_GameViewPortSize = { 0.0f, 0.0f };

		///

		std::string m_FilePath = "";

		std::string m_FileName = "";

		///

		Ref<Scene> m_Scene;

		FileBrowserFlags m_FileBrowserState = FileBrowserFlags::None;

		SelectionFlags m_SelectionFlags = SelectionFlags::None;

		///

		size_t m_IDBuffer = 0;

		bool isSceneViewFocused = false;

		bool isGameViewFocused = false;

		///

		std::shared_ptr<EditorConsole> m_EditorConsole = nullptr;

		std::shared_ptr<ImGui::FileBrowser> m_FileBrowser = nullptr;

		std::unique_ptr<BuildPanel> m_BuildPanel = nullptr;

		std::unique_ptr<AnimationPanel> m_AnimationPanel = nullptr;

		std::unique_ptr<ActorCreationWindow> m_ActorCreationWindow = nullptr;

		std::unique_ptr<SettingsWindow> m_SettingsWindow = nullptr;

		///

		//TEMP 
		Ref<Actor> m_Actor = nullptr;
		Ref<Actor> m_CameraActor = nullptr;
		Ref<Actor> m_SelectedActor = nullptr;

		Ref<Texture2D> m_Texture = nullptr;
		Ref<Texture2D> m_SheetTexture = nullptr;
		Ref<SubTexture2D> m_HouseSubTexture = nullptr;
		Ref<SubTexture2D> m_FieldSubTexture = nullptr;
	};
}

