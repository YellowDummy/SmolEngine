#pragma once
#include "SmolEngineCore.h"
#include "Core/Layer.h"

#include "Core/ImGui/EditorConsole.h"
#include "Core/ImGui/EditorPanels.h"
#include "Core/ImGui/BuildPanel.h"

#include "Core/ImGui/AnimationPanel.h"
#include "Core/Animation/AnimationClip.h"

#include <imgui/imgui.h>

#include "Core/ImGui/FileBrowser/imfilebrowser.h"

namespace SmolEngine
{
	struct TransformComponent;

	struct Texture2DComponent;

	struct HeadComponent;

	struct Body2DComponent;

	struct CameraComponent;

	struct AudioSourceComponent;

	struct Animation2DComponent;

	struct CanvasComponent;

	struct BehaviourComponent;

	class Actor;

	///

	enum class FileBrowserFlags : uint16_t
	{
		None = 0,

		//Textures

		Load_Texture2D,

		//Animation

		Load_Animation_Clip,
		Load_Animation_Clip_Inspector,

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

	enum class SelectionFlags: uint16_t
	{
		None = 0,
		Inspector,
		Actions
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

		/// Draw

		void DrawInfo(HeadComponent* head);

		void DrawTransform(TransformComponent* transform);

		void DrawTexture(Texture2DComponent* texture);

		void DrawBody2D(Body2DComponent* rb);

		void DrawCamera(CameraComponent* camera);

		void DrawAudioSource(AudioSourceComponent* audio);

		void DrawAnimation2D(Animation2DComponent* anim);

		void DrawCanvas(CanvasComponent* canvas);

		void DrawBehaviorComponent(BehaviourComponent* behaviour);

		///

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

		size_t m_IDBuffer = 0;

		///

		Ref<WorldAdmin> m_Scene;

		FileBrowserFlags m_FileBrowserState = FileBrowserFlags::None;

		SelectionFlags m_SelectionFlags = SelectionFlags::None;

		///

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

		static std::string m_TempActorName;

		static std::string m_TempActorTag;

		static std::string m_TempString;

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

