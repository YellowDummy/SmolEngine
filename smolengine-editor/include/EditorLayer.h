#pragma once

#include "SmolEngineCore.h"
#include "RendererPanel.h"
#include "TexturesLoader.h"
#include "FileExplorer.h"
#include "Core/Layer.h"
#include "Core/EditorConsole.h"
#include "MaterialLibraryInterface.h"
#include "ECS/Components/BaseComponent.h"

#include <Frostium3D/EditorCamera.h>
#include <Frostium3D/Libraries/imgui/imgui.h>
#include <Frostium3D/Libraries/imgizmo/src/ImGuizmo.h>
#include <Frostium3D/Libraries/glm/glm/glm.hpp>
#include <Frostium3D/EditorCamera.h>
#include <Frostium3D/Common/Texture.h>

namespace SmolEngine
{
	struct TransformComponent;
	struct Texture2DComponent;
	struct HeadComponent;
	struct Rigidbody2DComponent;
	struct CameraComponent;
	struct AudioSourceComponent;
	struct Animation2DComponent;
	struct CanvasComponent;
	struct BehaviourComponent;
	struct Light2DSourceComponent;
	struct MeshComponent;
	struct DirectionalLightComponent;
	struct PointLightComponent;
	struct SpotLightComponent;
	struct RigidbodyComponent;
	struct StaticbodyComponent;

	class Actor;
	class EditorCamera;

	enum class SelectionFlags: uint16_t
	{
		None = 0,
		Inspector,
		Actions,
		MaterialLib
	};

	class EditorLayer: public Layer
	{
	public:

		EditorLayer(EditorCamera* camera)
			:m_Camera(camera), Layer("EditorLayer") {}

		~EditorLayer() = default;

		// Override
		void OnAttach() override;
		void OnDetach() override;
		void OnBeginFrame(DeltaTime deltaTime) override;
		void OnEndFrame(DeltaTime deltaTime) override;
		void OnUpdate(DeltaTime deltaTime) override;
		void OnEvent(Event& event) override;
		void OnImGuiRender() override;

		// Draw
		void DrawActor(Actor* actor, uint32_t index = 0);
		void DrawToolsBar();
		void DrawSceneTetxure();
		void DrawInfo(HeadComponent* head);
		void DrawTransform(TransformComponent* transform);
		void DrawTexture(Texture2DComponent* texture);
		void DrawRigidBody2D(Rigidbody2DComponent* rb);
		void DrawCamera(CameraComponent* camera);
		void DrawAudioSource(AudioSourceComponent* audio);
		void DrawAnimation2D(Animation2DComponent* anim);
		void DrawCanvas(CanvasComponent* canvas);
		void DrawBehaviorComponent(std::vector<OutValue>& outValues);
		void DrawLight2D(Light2DSourceComponent* light);
		void DrawSceneView(bool enabled);
		void DrawInspector();
		void DrawHierarchy();
		void DrawMeshComponent(MeshComponent* meshComponent);
		void DrawMeshInspector(bool& show);
		void DrawPointLightComponent(PointLightComponent* light);
		void DrawSpotLightComponent(SpotLightComponent* light);
		void DrawRigidBodyComponent(RigidbodyComponent* component);

		void DrawComponents();
		void DrawComponentPopUp();
		void DrawScriptPopUp();

		static bool FileExtensionCheck(std::string& path, const std::string& ext);

	private:
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

		void ResetSelection();
		void DrawScriptComponent(uint32_t index);
		void CheckActor(Actor* actor);
		void CheckGameCameraState();

		// Callbacks
		void OnFileSelected(const std::string& path, const std::string& ext, int fileSize);
		void OnFileDeleted(const std::string& path, const std::string& ext);

	private:

		bool                                        m_IsSceneViewFocused = false;
		bool                                        m_IsGameViewFocused = false;
		bool                                        m_GizmoEnabled = true;
		bool                                        m_SnapEnabled = false;
		bool                                        m_GameCameraEnabled = false;
		TexturesLoader*                             m_TexturesLoader = nullptr;
		FileExplorer*                               m_FileExplorer = nullptr;
		WorldAdmin*                                 m_World = nullptr;
		EditorCamera*                               m_Camera = nullptr;
		EditorConsole*                              m_Console = nullptr;
		RendererPanel*                              m_RendererPanel = nullptr;
		Actor*                                      m_SelectedActor = nullptr;
		MaterialLibraryInterface*                   m_MaterialLibraryInterface = nullptr;
		SelectionFlags                              m_SelectionFlags = SelectionFlags::None;
		glm::vec2                                   m_SceneViewPort = { 0.0f, 0.0f };
		std::string                                 m_FilePath = "";
		std::string                                 m_FileName = "";
		uint32_t                                    m_IDBuffer = 0;
		std::vector<Actor*>                         m_DisplayedActors;

		inline static std::string                   m_TempActorName = "";
		inline static std::string                   m_TempActorTag = "";
		inline static std::string                   m_TempString = "";               
		inline static ImGuizmo::OPERATION           m_GizmoOperation = ImGuizmo::OPERATION::TRANSLATE;

		friend class MaterialLibraryInterface;
	};
}

