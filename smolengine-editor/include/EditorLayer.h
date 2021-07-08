#pragma once

#include "SmolEngineCore.h"
#include "RendererInspector.h"
#include "TextureInspector.h"
#include "TexturesLoader.h"
#include "FileExplorer.h"
#include "EditorConsole.h"
#include "ViewPort.h"
#include "Core/Layer.h"
#include "MaterialInspector.h"
#include "ECS/Components/BaseComponent.h"

#include <Frostium3D/EditorCamera.h>
#include <Frostium3D/Libraries/imgui/imgui.h>
#include <Frostium3D/Libraries/glm/glm/glm.hpp>
#include <Frostium3D/EditorCamera.h>
#include <Frostium3D/Primitives/Texture.h>

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
		MaterialView,
		TextureView
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
		void DrawActor(Ref<Actor>& actor, uint32_t index = 0);
		void DrawToolsBar();
		void DrawMeshPanel();
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
		void DrawMeshPrimitive(uint32_t type, const std::string& title, Texture* icon);
		void CheckActor(Ref<Actor>& actor);

		// Callbacks
		void OnFileSelected(const std::string& path, const std::string& ext, int fileSize);
		void OnFileDeleted(const std::string& path, const std::string& ext);

	private:
		TexturesLoader*                             m_TexturesLoader = nullptr;
		FileExplorer*                               m_FileExplorer = nullptr;
		WorldAdmin*                                 m_World = nullptr;
		ViewPort*                                   m_SceneView = nullptr;
		ViewPort*                                   m_GameView = nullptr;
		EditorCamera*                               m_Camera = nullptr;
		EditorConsole*                              m_Console = nullptr;
		RendererInspector*                          m_RendererInspector = nullptr;
		TextureInspector*                           m_TextureInspector = nullptr;
		Ref<Actor>                                  m_SelectedActor = nullptr;
		MaterialInspector*                          m_MaterialInspector = nullptr;
		SelectionFlags                              m_SelectionFlags = SelectionFlags::None;
		std::string                                 m_FilePath = "";
		std::string                                 m_FileName = "";
		uint32_t                                    m_IDBuffer = 0;
		std::vector<Ref<Actor>>                     m_DisplayedActors;

		inline static std::string                   m_TempActorName = "";
		inline static std::string                   m_TempActorTag = "";
		inline static std::string                   m_TempString = "";               

		friend class MaterialInspector;
		friend class SceneView;
	};
}

