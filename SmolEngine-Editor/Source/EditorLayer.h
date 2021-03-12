#pragma once
#include "SmolEngineCore.h"
#include "Core/Layer.h"

#include "ImGui/EditorConsole.h"
#include "ImGui/EditorPanels.h"
#include "ImGui/BuildPanel.h"
#include "ImGui/FileBrowser/imfilebrowser.h"
#include "ImGui/AnimationPanel.h"
#include "ImGui/MaterialLibraryInterface.h"

#include "ECS/Components/BaseComponent.h"

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <ImGuizmo.h>


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
	struct Light2DSourceComponent;
	struct MeshComponent;
	struct DirectionalLightComponent;
	struct PointLightComponent;

	class Actor;
	class Mesh;
	class Framebuffer;
	class EditorCamera;

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

	class EditorLayer: public Layer
	{
	public:

		EditorLayer()
			:Layer("EditorLayer") {}

		~EditorLayer() {}

		// Override

		void OnAttach() override;

		void OnDetach() override;

		void OnUpdate(DeltaTime deltaTime) override;

		void OnEvent(Event& event) override;

		void OnImGuiRender() override;

		// Draw

		void DrawInfo(HeadComponent* head);

		void DrawTransform(TransformComponent* transform);

		void DrawTexture(Texture2DComponent* texture);

		void DrawBody2D(Body2DComponent* rb);

		void DrawCamera(CameraComponent* camera);

		void DrawAudioSource(AudioSourceComponent* audio);

		void DrawAnimation2D(Animation2DComponent* anim);

		void DrawCanvas(CanvasComponent* canvas);

		void DrawBehaviorComponent(std::vector<OutValue>& outValues);

		void DrawLight2D(Light2DSourceComponent* light);

		void DrawSceneView(bool enabled);

		void DrawGameView(bool enabled);

		void DrawInspector();

		void DrawHierarchy();

		void DrawMeshComponent(MeshComponent* meshComponent);

		void DrawDirectionalLightComponent(DirectionalLightComponent* light);

		void DrawMeshInspector(bool& show);

		void DrawPointLightComponent(PointLightComponent* light);

		// Helpers

		void UpdateFileBrowser(bool& showAnimPanel);

	private:

		void ResetFileBrowser();

		template<typename T>
		bool IsCurrentComponent(uint32_t index)
		{
			if (m_Scene->GetActiveScene()->HasComponent<T>(*m_SelectedActor.get()))
			{
				auto comp = m_Scene->GetActiveScene()->GetComponent<T>(*m_SelectedActor.get());
				BaseComponent* baseComp = static_cast<BaseComponent*>(comp);
				return baseComp->ComponentID == index;
			}

			return false;
		}

		void DrawScriptComponent(uint32_t index);

	private:

		glm::vec2 m_GameViewSize = { 0.0f, 0.0f };
		glm::vec2 m_SceneViewSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewPortSize = {0.0f, 0.0f};
		glm::vec2 m_GameViewPortSize = { 0.0f, 0.0f };

		std::string m_FilePath = "";
		std::string m_FileName = "";

		size_t m_IDBuffer = 0;

		WorldAdmin*       m_Scene = nullptr;
		Ref<EditorCamera> m_Camera = nullptr;
		FileBrowserFlags m_FileBrowserState = FileBrowserFlags::None;
		SelectionFlags m_SelectionFlags = SelectionFlags::None;

		bool isSceneViewFocused = false;
		bool isGameViewFocused = false;
		bool m_GizmoEnabled = true;

		std::shared_ptr<EditorConsole> m_EditorConsole = nullptr;
		std::shared_ptr<ImGui::FileBrowser> m_FileBrowser = nullptr;

		std::unique_ptr<MaterialLibraryInterface> m_MaterialLibraryInterface = nullptr;
		std::unique_ptr<BuildPanel> m_BuildPanel = nullptr;
		std::unique_ptr<AnimationPanel> m_AnimationPanel = nullptr;
		std::unique_ptr<SettingsWindow> m_SettingsWindow = nullptr;

		inline static std::string m_TempActorName = "";
		inline static std::string m_TempActorTag = "";
		inline static std::string m_TempString = "";

		inline static ImGuizmo::OPERATION m_GizmoOperation = ImGuizmo::OPERATION::TRANSLATE;

		//TEMP 

		Mesh* m_SelectedMesh = nullptr;

		Ref<Actor> m_Actor = nullptr;
		Ref<Actor> m_CameraActor = nullptr;
		Ref<Actor> m_SelectedActor = nullptr;

		Ref<Texture> m_Texture = nullptr;
		Ref<Texture> m_SheetTexture = nullptr;

		Ref<SubTexture2D> m_HouseSubTexture = nullptr;
		Ref<SubTexture2D> m_FieldSubTexture = nullptr;
	};
}

