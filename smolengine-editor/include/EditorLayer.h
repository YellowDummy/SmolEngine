#pragma once
#include "SmolEngineCore.h"
#include "FileManager.h"
#include "Core/Layer.h"
#include "Core/MaterialLibraryInterface.h"
#include "Core/EditorConsole.h"
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
	struct RigidbodyComponent;
	struct StaticbodyComponent;

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

		~EditorLayer() = default;

		void LoadAssets();

		// Override
		void OnAttach() override;
		void OnDetach() override;
		void OnBeginFrame(Frostium::DeltaTime deltaTime) override;
		void OnUpdate(Frostium::DeltaTime deltaTime) override;
		void OnEvent(Frostium::Event& event) override;
		void OnImGuiRender() override;

		// Draw
		void DrawToolsBar();
		void DrawSceneTetxure();
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
		void DrawInspector();
		void DrawHierarchy();
		void DrawMeshComponent(MeshComponent* meshComponent);
		void DrawDirectionalLightComponent(DirectionalLightComponent* light);
		void DrawMeshInspector(bool& show);
		void DrawPointLightComponent(PointLightComponent* light);
		void DrawRigidBodyComponent(RigidbodyComponent* component);
		void DrawStaticBodyComponent(StaticbodyComponent* component);

	private:

		bool FileExtensionCheck(std::filesystem::path* path, const std::string& name, std::string& strPath);

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

		FileManager*                                m_FileManager = nullptr;
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

		// UI
		Frostium::Texture                           m_PlayButton{};
		Frostium::Texture                           m_StopButton{};
		Frostium::Texture                           m_MoveButton{};
		Frostium::Texture                           m_ScaleButton{};
		Frostium::Texture                           m_RotateButton{};
		Frostium::Texture                           m_SearchButton{};
		Frostium::Texture                           m_RemoveButton{};

		inline static std::string                   m_TempActorName = "";
		inline static std::string                   m_TempActorTag = "";
		inline static std::string                   m_TempString = "";               
		inline static ImGuizmo::OPERATION           m_GizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	};
}

