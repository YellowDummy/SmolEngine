#pragma once
#include "SmolEngineCore.h"
#include "Core/Layer.h"
#include "Core/MaterialLibraryInterface.h"
#include "Core/EditorConsole.h"
#include "ECS/Components/BaseComponent.h"

#include <Frostium3D/EditorCamera.h>
#include <Frostium3D/Libraries/imgui/imgui.h>
#include <Frostium3D/Libraries/imgizmo/src/ImGuizmo.h>
#include <Frostium3D/Libraries/glm/glm/glm.hpp>
#include <Frostium3D/EditorCamera.h>

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
	class EditorCamera;

	enum class SelectionFlags: uint16_t
	{
		None = 0,
		Inspector,
		Actions
	};

	class EditorLayer: public Layer
	{
	public:

		EditorLayer(Frostium::EditorCamera* camera)
			:m_Camera(camera), Layer("EditorLayer") {}

		~EditorLayer() {}

		// Override
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(Frostium::DeltaTime deltaTime) override;
		void OnEvent(Frostium::Event& event) override;
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

	private:

		void ResetFileBrowser();

		template<typename T>
		bool IsCurrentComponent(uint32_t index)
		{
			if (m_World->GetActiveScene()->HasComponent<T>(m_SelectedActor))
			{
				auto comp = m_World->GetActiveScene()->GetComponent<T>(m_SelectedActor);
				BaseComponent* baseComp = dynamic_cast<BaseComponent*>(comp);
				return baseComp->ComponentID == index;
			}

			return false;
		}

		void DrawScriptComponent(uint32_t index);

	private:

		bool                                        m_IsSceneViewFocused = false;
		bool                                        m_IsGameViewFocused = false;
		bool                                        m_GizmoEnabled = true;

		WorldAdmin*                                 m_World = nullptr;
		Frostium::EditorCamera*                     m_Camera = nullptr;
		EditorConsole*                              m_Console = nullptr;
		Actor*                                      m_SelectedActor = nullptr;
		std::unique_ptr<MaterialLibraryInterface>   m_MaterialLibraryInterface = nullptr;
		SelectionFlags                              m_SelectionFlags = SelectionFlags::None;
		glm::vec2                                   m_GameViewSize = { 0.0f, 0.0f };
		glm::vec2                                   m_SceneViewSize = { 0.0f, 0.0f };
		glm::vec2                                   m_ViewPortSize = {0.0f, 0.0f};
		glm::vec2                                   m_GameViewPortSize = { 0.0f, 0.0f };
		std::string                                 m_FilePath = "";
		std::string                                 m_FileName = "";
		size_t                                      m_IDBuffer = 0;

		inline static std::string                   m_TempActorName = "";
		inline static std::string                   m_TempActorTag = "";
		inline static std::string                   m_TempString = "";               
		inline static ImGuizmo::OPERATION           m_GizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	};
}

