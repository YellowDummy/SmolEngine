#include "stdafx.h"
#include "EditorLayer.h"
#include "ImGuiExtension.h"

#include "Audio/AudioClip.h"

#include "ECS/WorldAdmin.h"
#include "ECS/ComponentHandler.h"
#include "ECS/Actor.h"
#include "ECS/ComponentsCore.h"
#include "ECS/Systems/RendererSystem.h"
#include "ECS/Systems/Physics2DSystem.h"
#include "ECS/Systems/AudioSystem.h"
#include "ECS/Systems/CameraSystem.h"
#include "ECS/Systems/UISystem.h"
#include "ECS/Systems/ScriptingSystem.h"
#include "ECS/Components/Singletons/AudioEngineSComponent.h"
#include "ECS/Components/Singletons/Box2DWorldSComponent.h"
#include "ECS/Components/Singletons/ProjectConfigSComponent.h"
#include "ECS/Components/Singletons/JobsSystemStateSComponent.h"
#include "ECS/Scene.h"

#include <Frostium3D/Libraries/imgui/imgui.h>
#include <Frostium3D/Libraries/imgui/imgui_internal.h>
#include <Frostium3D/Libraries/glm/glm/glm.hpp>
#include <Frostium3D/Libraries/glm/glm/gtc/matrix_transform.hpp>
#include <Frostium3D/Utils/Utils.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <Frostium3D/Libraries/glm/glm/gtx/quaternion.hpp>
#include <Frostium3D/Libraries/glm/glm/gtx/matrix_decompose.hpp>
#include <Frostium3D/Libraries/glm/glm/gtc/type_ptr.hpp>
#include <Libraries/entt/entt.hpp>
#include <fstream>

#include <ImGUI/imgui_internal.h>

namespace SmolEngine
{
	static bool showConsole = true;
	static bool showGameView = false;
	static bool showRendererPanel = false;
	static bool showMeshInspector = false;

	void EditorLayer::OnAttach()
	{
		m_Console = new EditorConsole();
		m_TexturesLoader = new TexturesLoader();
		m_RendererInspector = new RendererInspector();
		m_TextureInspector = new TextureInspector();
		m_MaterialInspector = new MaterialInspector();
		m_World = WorldAdmin::GetSingleton();
		m_World->CreateScene(std::string("TestScene2.s_scene"));

		m_SceneView = new SceneView(this);
		m_GameView = new GameView();

		m_FileExplorer = new FileExplorer();
		m_FileExplorer->Create(Engine::GetEngine()->GetAssetsFolder());
		m_FileExplorer->SetOnFileSelectedCallback(std::bind_front(&EditorLayer::OnFileSelected, this));
		m_FileExplorer->SetOnFileDeletedCallaback(std::bind_front(&EditorLayer::OnFileDeleted, this));

		{
			ImGui::GetStyle().FrameRounding = 4.0f;
			ImGui::GetStyle().GrabRounding = 4.0f;

			ImVec4* colors = ImGui::GetStyle().Colors;
			colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
			colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
			colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
			colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
			colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
			colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
			colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
			colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
			colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
			colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
			colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
			colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.5f);
			colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
			colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
			colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
			colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
			colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
			colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
			colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
			colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
			colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
			colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
			colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
			colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
			colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
			colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
			colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
			colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
			colors[ImGuiCol_Tab] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
			colors[ImGuiCol_TabHovered] = ImVec4(0.1f, 0.1f, 0.1f, 0.1f);
			colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
			colors[ImGuiCol_TabUnfocused] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
			colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
			colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
			colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
			colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
			colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
			colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
			colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
			colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
			colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

			// Buttons
			colors[ImGuiCol_Button] = ImVec4(0.21f, 0.68f, 0.80f, 1.00f);
			colors[ImGuiCol_ButtonHovered] = ImVec4(0.45f, 0.70f, 0.82f, 1.00f);
			colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);

			colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
			colors[ImGuiCol_DockingPreview] = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);
		}

		SLog::s_Instance->SetOnPrintCallback([&](const std::string&& msg, LogType type)
		{
			switch (type)
			{
			case LogType::Info:
			{
				m_Console->AddMessageInfo(msg);
				break;
			}
			case LogType::Warning:
			{
				m_Console->AddMessageWarn(msg);
				break;
			}
			case LogType::Error:
			{
				m_Console->AddMessageError(msg);
				break;
			}
			}
		});
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnBeginFrame(DeltaTime deltaTime)
	{
		if(m_SceneView->IsFocused() && !m_GameView->IsFocused())
		{
			m_Camera->OnUpdate(deltaTime);
		}

		BeginSceneInfo bSeneInfo = {};
		bSeneInfo.Update(m_Camera);
		m_World->SetBeginSceneInfo(&bSeneInfo);
	}

	void EditorLayer::OnEndFrame(DeltaTime deltaTime)
	{
		if (m_GameView->IsActive())
		{
			m_GameView->Render();
		}

		if (m_World->IsInPlayMode() == false)
		{
			RendererSystem::DebugDraw(m_RendererInspector->GetDebugState());
		}
	}

	void EditorLayer::OnUpdate(DeltaTime deltaTime)
	{

	}

	void EditorLayer::OnEvent(Event& e)
	{
		if (m_SceneView->IsFocused() && !m_GameView->IsFocused())
		{
			m_Camera->OnEvent(e);
		}
	}

	void EditorLayer::OnImGuiRender()
	{

		static bool p_open = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &p_open, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		m_SceneView->Draw();
		m_GameView->Draw();

		m_RendererInspector->OnUpdate(showRendererPanel);
		m_Console->Update(showConsole);
		m_FileExplorer->Update();

		DrawToolsBar();
		DrawHierarchy();
		DrawInspector();

		ImGui::End();

	}

	void EditorLayer::DrawActor(Ref<Actor>& actor, uint32_t index)
	{
		if (m_SelectedActor == actor)
			ImGui::PushStyleColor(ImGuiCol_Text, { 0.1f, 0.3f, 1.0f, 1.0f });

		bool open = ImGui::TreeNodeEx(actor->GetName().c_str(), m_SelectedActor == actor ? ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_OpenOnArrow : ImGuiTreeNodeFlags_OpenOnArrow);

		if (m_SelectedActor == actor)
			ImGui::PopStyleColor();

		if (ImGui::IsItemClicked(1))
		{
			ResetSelection();
			m_SelectionFlags = SelectionFlags::Actions;
			m_SelectedActor = actor;
		}

		if (ImGui::IsItemClicked())
		{
			if (m_SelectedActor != nullptr)
				m_SelectedActor->GetInfo()->bShowComponentUI = false;

			ResetSelection();
			m_SelectionFlags = SelectionFlags::Inspector;
			m_SelectedActor = actor;

			if (ImGui::IsMouseDoubleClicked(0))
			{
				glm::vec3 pos = m_SelectedActor->GetComponent<TransformComponent>()->WorldPos;
				m_Camera->SetDistance(6.0f);
				m_Camera->SetPosition(pos);
			}
		}

		if (ImGui::BeginDragDropSource())
		{
			m_IDBuffer = actor->GetID();
			ImGui::SetDragDropPayload("ActorDragAndDrop", &m_IDBuffer, sizeof(Actor));
			ImGui::Text(actor->GetName().c_str());
			ImGui::EndDragDropSource();
			m_IDBuffer = index;
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ActorDragAndDrop"))
			{
				uint32_t id = *static_cast<uint32_t*>(payload->Data);
				Ref<Actor> target = m_World->GetActiveScene()->FindActorByID(id);
				if (target != actor)
				{
					actor->SetChild(target);
				}
			}

			ImGui::EndDragDropTarget();
		}

		if (open)
		{
			uint32_t i = 0;
			for (auto child : actor->GetChilds())
			{
				DrawActor(child, i);
				i++;
			}

			ImGui::TreePop();
		}
	}

	void EditorLayer::DrawToolsBar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 10.0f, 10.0f });
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Close"))
				{
					Engine::GetEngine()->Shutdown();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Scene"))
			{
				if (!m_World->IsInPlayMode())
				{
					if (ImGui::MenuItem("New"))
					{
						const auto& result = Utils::SaveFile("SmolEngine Scene (*.s_scene)\0*.s_scene\0", "new_scene.s_scene");
						if (result.has_value())
						{
							m_SelectedActor = nullptr;
							m_World->CreateScene(result.value());
						}
					}

					if (ImGui::MenuItem("Save"))
					{
						if (!m_World->SaveCurrentScene())
						{
							NATIVE_ERROR("Couldn't save current scene!");
						}
					}

					if (ImGui::MenuItem("Save as"))
					{
						const auto& result = Utils::SaveFile("SmolEngine Scene (*.s_scene)\0*.s_scene\0", "new_scene.s_scene");
						if (result.has_value())
						{
							m_SelectedActor = nullptr;
							std::filesystem::path path = result.value();
							m_World->GetActiveScene()->GetSceneState()->Name = path.filename().stem().string();
							m_World->SaveScene(result.value());
						}
					}

					if (ImGui::MenuItem("Load"))
					{
						const auto& result = Utils::OpenFile("SmolEngine Scene (*.s_scene)\0*.s_scene\0");
						if (result.has_value())
						{
							m_SelectedActor = nullptr;
							m_World->LoadScene(result.value());
						}
					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Console"))
					showConsole = true;

				if (ImGui::MenuItem("Preview"))
					m_GameView->SetActive(true);

				if (ImGui::MenuItem("Renderer Settings"))
					showRendererPanel = true;

				ImGui::EndMenu();
			}

		}
		ImGui::EndMainMenuBar();
		ImGui::PopStyleVar();

	}

	void EditorLayer::DrawMeshPanel()
	{
		if (ImGui::CollapsingHeader("Primitives", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::NewLine();
			DrawMeshPrimitive((uint32_t)MeshComponent::DefaultMeshType::Cube, "Cube", &m_TexturesLoader->m_CubeIcon);
			DrawMeshPrimitive((uint32_t)MeshComponent::DefaultMeshType::Sphere, "Sphere", &m_TexturesLoader->m_SphereIcon);
			DrawMeshPrimitive((uint32_t)MeshComponent::DefaultMeshType::Capsule, "Capsule", &m_TexturesLoader->m_CapsuleIcon);
			DrawMeshPrimitive((uint32_t)MeshComponent::DefaultMeshType::Torus, "Torus", &m_TexturesLoader->m_TorusIcon);
			ImGui::NewLine();
		}
	}

	void EditorLayer::DrawInfo(HeadComponent* head)
	{
		m_TempActorName = head->Name;
		m_TempActorTag = head->Tag;

		if (ImGui::Extensions::InputRawString("Name", m_TempActorName))
			m_SelectedActor->SetName(m_TempActorName);

		ImGui::Extensions::InputString("Tag", head->Tag, m_TempActorTag);
		ImGui::Extensions::CheckBox("Enabled", head->bEnabled, 130.0f, "HeadPanel");
	}

	void EditorLayer::DrawTransform(TransformComponent* transform)
	{
		ImGui::Extensions::TransformComponent(transform->WorldPos, transform->Scale, transform->Rotation);
	}

	void EditorLayer::DrawTexture(Texture2DComponent* texture)
	{
		if (texture->Texture != nullptr)
		{
			ImGui::Extensions::ColorInput4("Color", texture->Color);
			ImGui::Extensions::InputInt("Layer", texture->LayerIndex, 130.0f, "TexturePanel");
			ImGui::NewLine();
			ImGui::Extensions::CheckBox("Enabled", texture->Enabled, 130.0f, "TexturePanel");
		}

		ImGui::NewLine();
		ImGui::SetCursorPosX(10);
		if (ImGui::Button("Load Texture", { ImGui::GetWindowWidth() - 20.0f, 30.0f }))
		{
			const auto& result = Utils::OpenFile("png (*png)\0*.png\0jpg (*jpg)\0*.jpg\0");
			if (result.has_value())
				ComponentHandler::ValidateTexture2DComponent(texture, result.value());
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FileBrowser"))
			{
				std::string& path = *(std::string*)payload->Data;
				if (EditorLayer::FileExtensionCheck(path, ".png") || EditorLayer::FileExtensionCheck(path, ".jpg"))
				{
					ComponentHandler::ValidateTexture2DComponent(texture, path);
				}
			}
			ImGui::EndDragDropTarget();
		}

	}

	void EditorLayer::DrawRigidBody2D(Rigidbody2DComponent* rb)
	{
		ImGui::Extensions::Combo("Type", "Static\0Kinematic\0Dynamic\0\0", rb->Body.m_Type);
		ImGui::Extensions::Combo("Shape", "Box\0Circle\0\0", rb->Body.m_ShapeType);
		ImGui::Extensions::InputInt("Layer", rb->Body.m_CollisionLayer);
		ImGui::NewLine();

		if (rb->Body.m_ShapeType == (int)Shape2DType::Box)
		{
			ImGui::Extensions::InputFloat2Base("Size", rb->Body.m_Shape);
			ImGui::NewLine();
		}

		if (rb->Body.m_ShapeType == (int)Shape2DType::Cirlce)
		{
			ImGui::Extensions::InputFloat("Radius", rb->Body.m_Radius);
			ImGui::Extensions::InputFloat2Base("Offset", rb->Body.m_Offset);
			ImGui::NewLine();
		}


		if (rb->Body.m_Type == 2 || rb->Body.m_Type == 1)
		{
			ImGui::Extensions::InputFloat("Inertia Moment", rb->Body.m_InertiaMoment);
			ImGui::Extensions::InputFloat("Gravity", rb->Body.m_GravityScale);
			ImGui::Extensions::InputFloat("Mass", rb->Body.m_Mass);
			ImGui::Extensions::InputFloat2Base("Mass Center", rb->Body.m_MassCenter);
			ImGui::NewLine();
			ImGui::Extensions::InputFloat("Restitution", rb->Body.m_Restitution);
			ImGui::Extensions::InputFloat("Friction", rb->Body.m_Friction);
			ImGui::Extensions::InputFloat("Density", rb->Body.m_Density);
			ImGui::Extensions::CheckBox("Bullet", rb->Body.m_IsBullet);
			ImGui::NewLine();

		}

		ImGui::Extensions::CheckBox("Trigger", rb->Body.m_IsTrigger);
		ImGui::Extensions::CheckBox("Awake", rb->Body.m_IsAwake);
		ImGui::Extensions::CheckBox("Allow Sleep", rb->Body.m_canSleep);
		ImGui::Extensions::CheckBox("Draw Shape", rb->ShowShape);
	}

	void EditorLayer::DrawCamera(CameraComponent* camera)
	{
		if (ImGui::Extensions::Combo("Type", "Perspective\0Ortho\0", camera->ImGuiType, 130.0f, "CameraComponent"))
			camera->eType = (CameraComponentType)camera->ImGuiType;

		ImGui::Extensions::InputFloat("Zoom", camera->ZoomLevel);
		ImGui::Extensions::InputFloat("FOV", camera->FOV);
		ImGui::Extensions::InputFloat("Near", camera->zNear);
		ImGui::Extensions::InputFloat("Far", camera->zFar);

		ImGui::NewLine();
		if(ImGui::Extensions::CheckBox("Primary", camera->bPrimaryCamera))
		{
			if (camera->bPrimaryCamera)
			{
				const size_t id = m_SelectedActor->GetID();
				entt::registry& reg = m_World->GetActiveScene()->GetRegistry();
				reg.view<HeadComponent, CameraComponent>().each([&](HeadComponent& head, CameraComponent& camera)
				{
					if (head.ActorID != id)
						camera.bPrimaryCamera = false;
				});
			}
		}
	}

	void EditorLayer::DrawAudioSource(AudioSourceComponent* audio)
	{
		for (auto& pair: audio->AudioClips)
		{
			auto& [key, clip] = pair;

			// Tree
			ImGui::NewLine();
			std::stringstream ss;
			ss << "Audio Clip #" << clip->ClipName;
			if (ImGui::TreeNodeEx(ss.str().c_str()))
			{
				ImGui::NewLine();
				if (ImGui::Extensions::InputRawString("Key Name", m_TempString))
				{
					if (!m_TempString.empty())
						AudioSystem::RenameClip(*audio, clip->ClipName, m_TempString);

					ImGui::TreePop();
					break;
				}

				ImGui::NewLine();
				ImGui::Extensions::InputFloat("Volume", clip->Volume);
				ImGui::NewLine();
				ImGui::Extensions::CheckBox("Default Clip", clip->isDefaultClip);
				ImGui::Extensions::CheckBox("Looping", clip->IsLooping);
				ImGui::Extensions::CheckBox("Is 3D", clip->B3D);

				ImGui::NewLine();
				if (ImGui::Extensions::SmallButton("Debug", "Play"))
				{
					AudioSystem::DebugPlay(clip, &AudioEngineSComponent::Get()->Engine);
				}

				ImGui::SameLine();
				if (ImGui::SmallButton("Stop"))
				{
					AudioSystem::DebugStop(clip, &AudioEngineSComponent::Get()->Engine);
				}
				ImGui::TreePop();
			}

			ImGui::Separator();
			m_TempString = "";

		}

		ImGui::NewLine();
		ImGui::NewLine();

		if (ImGui::Button("New Audio Clip", { ImGui::GetWindowWidth() - 20.0f, 30.0f }))
		{

		}
	}

	void EditorLayer::DrawAnimation2D(Animation2DComponent* anim)
	{

	}

	void EditorLayer::DrawCanvas(CanvasComponent* canvas)
	{
		
	}

	void EditorLayer::DrawBehaviorComponent(std::vector<OutValue>& outValues)
	{

	}

	void EditorLayer::DrawLight2D(Light2DSourceComponent* light)
	{
		ImGui::Extensions::InputFloat2Base("Offset", light->Offset, 130.0f, "2DLightPanel");
		ImGui::Extensions::InputFloat("Intensity", light->Intensity, 130.0f, "2DLightPanel");
		ImGui::Extensions::InputFloat("Radius", light->Radius, 130.0f, "2DLightPanel");
		ImGui::Extensions::ColorInput4("Color", light->Color, 130.0f, "2DLightPanel");

		ImGui::NewLine();

		ImGui::Extensions::CheckBox("Enabled", light->IsEnabled, 130.0f, "2DLightPanel");
	}

	void EditorLayer::DrawInspector()
	{
		ImGui::Begin("Inspector");
		{
			ImGui::BeginChild("InspectorChild");
			{
				ImGui::SetWindowFontScale(0.9f);
				if (m_SelectionFlags != SelectionFlags::Inspector)
				{
					if (m_SelectionFlags == SelectionFlags::MaterialView)
					{
						m_MaterialInspector->Update();
					}
					else if (m_SelectionFlags == SelectionFlags::TextureView)
					{
						m_TextureInspector->Update();
					}
					else
					{
						DrawMeshPanel();
					}

					ImGui::EndChild();
					ImGui::End();
					return;
				}
				else if(m_SelectedActor && m_SelectionFlags == SelectionFlags::Inspector)
				{
					std::stringstream ss;

					if (ImGui::Button("Add Component"))
						ImGui::OpenPopup("AddComponentPopUp");

					ImGui::SameLine();
					if (ImGui::Button("Add C++ Script"))
						ImGui::OpenPopup("AddCScriptPopUp");

					DrawScriptPopUp();
					DrawComponentPopUp();

					DrawComponents();
				}
			}
			ImGui::EndChild();
		}

		ImGui::End();
	}

	void EditorLayer::DrawHierarchy()
	{
		ImGui::Begin("Hierarchy");
		{
			ImGui::SetWindowFontScale(0.8f);
			ImGui::Image(m_TexturesLoader->m_SearchButton.GetImGuiTexture(), { 25, 25 }, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::SameLine();

			float pos = ImGui::GetCursorPosY();
			ImGui::SetCursorPosY(pos + 3);
			static char name[128];
			ImGui::InputTextWithHint("", "Search", name, IM_ARRAYSIZE(name));
			ImGui::Separator();

			std::string sceneStr = "Scene";
			SceneStateComponent* state = m_World->GetActiveScene()->GetSceneState();

			if (!state->Name.empty())
			{
				sceneStr = "Scene: " + state->Name;
			}

			ImGui::BeginChild("Scene");
			{
				if (ImGui::IsWindowHovered())
				{
					if (Input::IsMouseButtonPressed(MouseCode::Button1))
					{
						ImGui::OpenPopup("CreateActorPopUp");
					}

					if (Input::IsMouseButtonPressed(MouseCode::Button0))
					{
						showMeshInspector = false;
						m_SelectedActor = nullptr;
						m_SelectionFlags = SelectionFlags::None;
					}
				}

				if (ImGui::BeginPopup("CreateActorPopUp"))
				{
					ImGui::MenuItem("New Actor", NULL, false, false);
					ImGui::Separator();
					std::stringstream ss;

					if (ImGui::MenuItem("Empty Actor"))
					{
						ss << "DefaultActor" << state->Actors.size();
						m_SelectedActor = m_World->GetActiveScene()->CreateActor(ss.str());
						m_SelectionFlags = SelectionFlags::Inspector;
					}

					ImGui::EndPopup();
				}

				bool open = ImGui::TreeNodeEx(sceneStr.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ActorDragAndDrop"))
					{
						uint32_t id = *static_cast<uint32_t*>(payload->Data);
						Ref<Actor> parent = m_World->GetActiveScene()->FindActorByID(id);
						if (parent != nullptr)
							parent->RemoveChildAtIndex(m_IDBuffer);
						
						m_IDBuffer = 0;
					}

					ImGui::EndDragDropTarget();
				}

				if (open)
				{
					m_DisplayedActors.clear();
					m_World->GetActiveScene()->GetActors(m_DisplayedActors);

					for (auto obj : m_DisplayedActors)
						CheckActor(obj);

					for (auto& actor : m_DisplayedActors)
					{
						auto result = actor->GetName().find(name);
						if (result == std::string::npos)
						{
							continue;
						}

						DrawActor(actor);
					}

					ImGui::TreePop();
				}

				if (m_SelectionFlags == SelectionFlags::Actions)
				{
					ImGui::OpenPopup("ActionPopup");
					m_SelectionFlags = SelectionFlags::None;
				}

				if (ImGui::BeginPopup("ActionPopup"))
				{
					ImGui::MenuItem(m_SelectedActor->GetName().c_str(), NULL, false, false);
					ImGui::Separator();

					if (ImGui::MenuItem("Save")) {}
					if (ImGui::MenuItem("Save as", "Ctrl+O")) {}


					if (ImGui::MenuItem("Dublicate", "Ctrl+C"))
					{
						m_World->GetActiveScene()->DuplicateActor(m_SelectedActor);
					}

					if (ImGui::MenuItem("Delete"))
					{
						m_World->GetActiveScene()->DeleteActor(m_SelectedActor);
						m_SelectedActor = nullptr;
					}

					ImGui::EndPopup();
				}
			}
			ImGui::EndChild();
		}
		ImGui::End();
	}

	void EditorLayer::DrawMeshComponent(MeshComponent* comp)
	{
		if (comp->MeshPtr || comp->DefaulPtr)
		{
			ImGui::Extensions::CheckBox("Show", comp->bShow);
			ImGui::Extensions::CheckBox("Static", comp->bIsStatic);
			ImGui::NewLine();

			if (!showMeshInspector)
			{
				ImGui::SetCursorPosX(10);
				if (ImGui::Button("Mesh Inspector", { ImGui::GetWindowWidth() - 20.0f, 30.0f }))
				{
					showMeshInspector = true;
				}
			}

			DrawMeshInspector(showMeshInspector);
		}

		ImGui::SetCursorPosX(10);
		if (ImGui::Button("Load Mesh", { ImGui::GetWindowWidth() - 20.0f, 30.0f }))
		{
			const auto& result = Utils::OpenFile("glTF 2.0 (*gltf)\0*.gltf\0");
			if (result.has_value())
			{
				ComponentHandler::ValidateMeshComponent(comp, result.value());
			}
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FileBrowser"))
			{
				std::string& path = *(std::string*)payload->Data;
				if (FileExtensionCheck(path, ".gltf"))
				{
					ComponentHandler::ValidateMeshComponent(comp, path);
				}

			}
			ImGui::EndDragDropTarget();
		}

		ImGui::NewLine();
	}

	void EditorLayer::DrawMeshInspector(bool& show)
	{
		auto* comp = m_World->GetActiveScene()->GetComponent<MeshComponent>(m_SelectedActor);
		if (!comp)
			return;

		if (show)
		{
			Mesh* mesh = nullptr;
			if (comp->eDefaultType == MeshComponent::DefaultMeshType::None)
				mesh = comp->MeshPtr.get();
			else
				mesh = comp->DefaulPtr;


			uint32_t count = static_cast<uint32_t>(mesh->GetChildCount()) + 1;
			std::vector<Mesh*> meshes(count);
			for (uint32_t i = 0; i < count; ++i)
			{
				if (i == 0)
				{
					meshes[0] = mesh;
					continue;
				}

				meshes[i] = &mesh->GetChilds()[i - 1];
			}

			ImGui::Begin("Mesh Inspector", &show);
			{
				ImGui::Extensions::Text("Mesh & SubMeshes", "");

				for (uint32_t i = 0; i < static_cast<uint32_t>(meshes.size()); ++i)
				{
					Mesh* mesh = meshes[i];

					std::string name = mesh->GetName() + " #" +std::to_string(i);
					if (ImGui::CollapsingHeader(name.c_str()))
					{
						std::string id = name + "IDMat";
						ImGui::PushID(id.c_str());
						{
							ImGui::Extensions::Text("Material ID", std::to_string(mesh->GetMaterialID()));
							if (ImGui::Button("Select Material"))
							{
								const auto& result = Utils::OpenFile("SmolEngine Material (*s_material)\0*.s_material\0");
								if (result.has_value())
								{
									ComponentHandler::SetMeshMaterial(comp, mesh, result.value());
								}
							}

							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FileBrowser"))
								{
									std::string& path = *(std::string*)payload->Data;
									if (FileExtensionCheck(path, ".s_material"))
									{
										ComponentHandler::SetMeshMaterial(comp, mesh, path);
									}
								}
								ImGui::EndDragDropTarget();
							}

						}
						ImGui::PopID();
					}
				}
			}
			ImGui::End();
		}
	}

	void EditorLayer::DrawPointLightComponent(PointLightComponent* comp)
	{
		bool* is_active = (bool*)&comp->IsActive;

		ImGui::Extensions::CheckBox("Enabled", *is_active);
		ImGui::Extensions::InputFloat("Exposure", comp->Intensity);
		ImGui::Extensions::InputFloat("Radius", comp->Raduis);
		ImGui::Extensions::ColorInput4("Color", comp->Color);
	}

	void EditorLayer::DrawSpotLightComponent(SpotLightComponent* light)
	{
		bool* is_active = (bool*)&light->IsActive;
		glm::vec3* dir = (glm::vec3*)&light->Direction;

		ImGui::Extensions::CheckBox("Enabled", *is_active);
		ImGui::Extensions::InputFloat3Base("Direction", *dir);
		ImGui::Extensions::InputFloat("Exposure", light->Intensity);
		ImGui::Extensions::InputFloat("CutOff", light->CutOff);
		ImGui::Extensions::InputFloat("Outer CutOff", light->OuterCutOff);
		ImGui::Extensions::InputFloat("Bias", light->Bias);
		ImGui::Extensions::InputFloat("Radius", light->Raduis);
		ImGui::Extensions::ColorInput4("Color", light->Color);
	}

	void EditorLayer::DrawRigidBodyComponent(RigidbodyComponent* component)
	{
		ImGui::Extensions::Combo("Type", "Dynamic\0Static\0Kinematic\0", component->CreateInfo.StateIndex);
		ImGui::Extensions::Combo("Shape", "Box\0Sphere\0Capsule\0Mesh\0", component->CreateInfo.ShapeIndex);

		component->CreateInfo.eType = (RigidBodyType)component->CreateInfo.StateIndex;
		component->CreateInfo.eShape = (RigidBodyShape)component->CreateInfo.ShapeIndex;

		if (component->CreateInfo.eShape == RigidBodyShape::Box)
		{
			ImGui::Extensions::InputFloat("X", component->CreateInfo.BoxShapeInfo.X);
			ImGui::Extensions::InputFloat("Y", component->CreateInfo.BoxShapeInfo.Y);
			ImGui::Extensions::InputFloat("Z", component->CreateInfo.BoxShapeInfo.Z);
		}

		if (component->CreateInfo.eShape == RigidBodyShape::Sphere)
		{
			ImGui::Extensions::InputFloat("Radius", component->CreateInfo.SphereShape.Radius);
		}

		if (component->CreateInfo.eShape == RigidBodyShape::Capsule)
		{
			ImGui::Extensions::InputFloat("Radius", component->CreateInfo.CapsuleShapeInfo.Radius);
			ImGui::Extensions::InputFloat("Height", component->CreateInfo.CapsuleShapeInfo.Height);
		}

		if (component->CreateInfo.eShape == RigidBodyShape::Convex)
		{
			ImGui::NewLine();
			ImGui::Separator();
			ImGui::NewLine();

			if (!component->CreateInfo.ConvexShapeInfo.FilePath.empty())
			{
				std::filesystem::path p(component->CreateInfo.ConvexShapeInfo.FilePath);
				ImGui::SetCursorPosX(12);
				ImGui::Extensions::Text(p.filename().stem().u8string(), "");
			}

			ImGui::SetCursorPosX(10);
			if (ImGui::Button("Select Mesh", { ImGui::GetWindowWidth() - 20.0f, 20.0f }))
			{
				const auto& result = Utils::OpenFile("glTF 2.0 (*gltf)\0*.gltf\0");
				if (result.has_value())
					component->CreateInfo.ConvexShapeInfo.FilePath = result.value();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FileBrowser"))
				{
					std::string& path = *(std::string*)payload->Data;
					if (FileExtensionCheck(path, ".gltf"))
					{
						component->CreateInfo.ConvexShapeInfo.FilePath = path;
					}
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SetCursorPosX(10);
			if (ImGui::Button("Clear", { ImGui::GetWindowWidth() - 20.0f, 20.0f }))
			{
				component->CreateInfo.ConvexShapeInfo.FilePath = "";
			}

			ImGui::NewLine();
		}

		ImGui::Separator();
		ImGui::NewLine();

		if (component->CreateInfo.eType == RigidBodyType::Dynamic)
		{
			ImGui::Extensions::InputFloat("Mass", component->CreateInfo.Mass);
		}

		ImGui::Extensions::InputFloat("Friction", component->CreateInfo.Friction);
		ImGui::Extensions::InputFloat("Restitution", component->CreateInfo.Restitution);
		ImGui::Extensions::InputFloat("Linear Damping", component->CreateInfo.LinearDamping);
		ImGui::Extensions::InputFloat("Angular Damping", component->CreateInfo.AngularDamping);
		ImGui::Extensions::InputFloat("Rolling Friction", component->CreateInfo.RollingFriction);
		ImGui::Extensions::InputFloat("Spinning Friction", component->CreateInfo.SpinningFriction);

		ImGui::NewLine();
	}

	void EditorLayer::DrawComponents()
	{
		if (ImGui::CollapsingHeader("Head"))
		{
			ImGui::NewLine();
			auto info = m_World->GetActiveScene()->GetComponent<HeadComponent>(m_SelectedActor);
			DrawInfo(info);
		}

		if (ImGui::CollapsingHeader("Tranform"))
		{
			ImGui::NewLine();
			auto trans = m_World->GetActiveScene()->GetComponent<TransformComponent>(m_SelectedActor);
			ImGui::Extensions::InputFloat3("Translation", trans->WorldPos);
			ImGui::Extensions::InputFloat3("Rotation", trans->Rotation);
			ImGui::Extensions::InputFloat3("Scale", trans->Scale, 1.0f);
		}

		for (uint32_t i = 0; i < m_SelectedActor->GetComponentsCount(); ++i)
		{
			if (IsCurrentComponent<Texture2DComponent>(i))
			{
				if (ImGui::CollapsingHeader("Texture 2D"))
				{
					ImGui::NewLine();
					auto component = m_World->GetActiveScene()->GetComponent<Texture2DComponent>(m_SelectedActor);
					DrawTexture(component);
				}
			}

			if (IsCurrentComponent<Rigidbody2DComponent>(i))
			{
				if (ImGui::CollapsingHeader("Rigidbody 2D"))
				{
					ImGui::NewLine();
					auto component = m_World->GetActiveScene()->GetComponent<Rigidbody2DComponent>(m_SelectedActor);
					DrawRigidBody2D(component);
				}
			}

			if (IsCurrentComponent<Animation2DComponent>(i))
			{
				if (ImGui::CollapsingHeader("Animation 2D"))
				{
					ImGui::NewLine();
					auto component = m_World->GetActiveScene()->GetComponent<Animation2DComponent>(m_SelectedActor);
					DrawAnimation2D(component);
				}
			}

			if (IsCurrentComponent<AudioSourceComponent>(i))
			{
				if (ImGui::CollapsingHeader("Audio Source"))
				{
					ImGui::NewLine();
					auto component = m_World->GetActiveScene()->GetComponent<AudioSourceComponent>(m_SelectedActor);
					DrawAudioSource(component);
				}
			}

			if (IsCurrentComponent<Light2DSourceComponent>(i))
			{
				if (ImGui::CollapsingHeader("Light 2D"))
				{
					ImGui::NewLine();
					auto component = m_World->GetActiveScene()->GetComponent<Light2DSourceComponent>(m_SelectedActor);
					DrawLight2D(component);
				}
			}

			if (IsCurrentComponent<MeshComponent>(i))
			{
				if (ImGui::CollapsingHeader("Mesh"))
				{
					ImGui::NewLine();
					auto component = m_World->GetActiveScene()->GetComponent<MeshComponent>(m_SelectedActor);
					DrawMeshComponent(component);
				}
			}

			if (IsCurrentComponent<PointLightComponent>(i))
			{
				if (ImGui::CollapsingHeader("Point Light"))
				{
					ImGui::NewLine();
					auto component = m_World->GetActiveScene()->GetComponent<PointLightComponent>(m_SelectedActor);
					DrawPointLightComponent(component);
				}
			}

			if (IsCurrentComponent<SpotLightComponent>(i))
			{
				if (ImGui::CollapsingHeader("Spot Light"))
				{
					ImGui::NewLine();
					auto component = m_World->GetActiveScene()->GetComponent<SpotLightComponent>(m_SelectedActor);
					DrawSpotLightComponent(component);
				}
			}

			if (IsCurrentComponent<CameraComponent>(i))
			{
				if (ImGui::CollapsingHeader("Camera"))
				{
					ImGui::NewLine();
					auto component = m_World->GetActiveScene()->GetComponent<CameraComponent>(m_SelectedActor);
					DrawCamera(component);
				}
			}

			if (IsCurrentComponent<RigidbodyComponent>(i))
			{
				if (ImGui::CollapsingHeader("Rigidbody"))
				{
					ImGui::NewLine();
					auto component = m_World->GetActiveScene()->GetComponent<RigidbodyComponent>(m_SelectedActor);
					DrawRigidBodyComponent(component);
				}
			}

			DrawScriptComponent(i);
		}

		ImGui::Separator();
		ImGui::NewLine();
	}

	void EditorLayer::DrawComponentPopUp()
	{
		if (ImGui::BeginPopup("AddComponentPopUp"))
		{
			ImGui::MenuItem("#Common", NULL, false, false);
			{
				ImGui::Separator();

				if (ImGui::MenuItem("Mesh"))
				{
					m_World->GetActiveScene()->AddComponent<MeshComponent>(m_SelectedActor);
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Texture"))
				{
					m_World->GetActiveScene()->AddComponent<Texture2DComponent>(m_SelectedActor);
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Camera"))
				{
					m_World->GetActiveScene()->AddComponent<CameraComponent>(m_SelectedActor);
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Canvas"))
				{
					m_World->GetActiveScene()->AddComponent<CanvasComponent>(m_SelectedActor);
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("AudioSource"))
				{
					m_World->GetActiveScene()->AddComponent<AudioSourceComponent>(m_SelectedActor);
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::MenuItem("#Lighting", NULL, false, false);
			{
				ImGui::Separator();

				if (ImGui::MenuItem("Spot Light"))
				{
					m_World->GetActiveScene()->AddComponent<SpotLightComponent>(m_SelectedActor);
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Point Light"))
				{
					m_World->GetActiveScene()->AddComponent<PointLightComponent>(m_SelectedActor);
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("2D Light"))
				{
					m_World->GetActiveScene()->AddComponent<Light2DSourceComponent>(m_SelectedActor);
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::MenuItem("#Physics", NULL, false, false);
			{
				ImGui::Separator();

				if (ImGui::MenuItem("RigidBody"))
				{
					auto comp = m_World->GetActiveScene()->AddComponent<RigidbodyComponent>(m_SelectedActor);
					ComponentHandler::ValidateRigidBodyComponent(comp, m_SelectedActor);
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("2D RigidBody"))
				{
					auto comp = m_World->GetActiveScene()->AddComponent<Rigidbody2DComponent>(m_SelectedActor);
					ComponentHandler::ValidateBody2DComponent(comp, m_SelectedActor);
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::EndPopup();
		}
	}

	void EditorLayer::DrawScriptPopUp()
	{
		if (ImGui::BeginPopup("AddCScriptPopUp"))
		{
			ImGui::MenuItem("New Script", NULL, false, false);
			ImGui::Separator();

			ScriptingSystemStateSComponent* state = ScriptingSystemStateSComponent::GetSingleton();

			for (const auto& [name, meta] : state->MetaMap)
			{
				if (ImGui::MenuItem(name.c_str()))
				{
					m_World->GetActiveScene()->AddScript(m_SelectedActor, name);
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
	}

	bool EditorLayer::FileExtensionCheck(std::string& path, const std::string& ext)
	{
		std::filesystem::path p(path);
		if (p.extension().filename() == ext)
		{
			return true;
		}

		return false;
	}

	void EditorLayer::ResetSelection()
	{
		m_TempActorTag = "";
		m_TempActorName = "";
		m_SelectedActor = nullptr;
		m_SelectionFlags = SelectionFlags::None;
		m_FileExplorer->ClearSelection();
	}

	void EditorLayer::DrawScriptComponent(uint32_t index)
	{
		if (m_World->GetActiveScene()->HasComponent<BehaviourComponent>(m_SelectedActor))
		{
			BehaviourComponent* comp = m_World->GetActiveScene()->GetComponent<BehaviourComponent>(m_SelectedActor);
			std::string scriptName = "";
			BehaviourComponent::OutData* data = nullptr;
			for (auto& [name, container] : comp->OutValues)
			{
				if (container.ScriptID == index)
				{
					data = &container;
					scriptName = name;
					break;
				}
			}

			if (!data)
				return;

			if (ImGui::CollapsingHeader(scriptName.c_str()))
			{
				ImGui::NewLine();
				ImGui::Extensions::Text("Script Type", "C++ Script");
				ImGui::Separator();
				ImGui::NewLine();

				for (auto& f : data->Floats)
					ImGui::Extensions::InputFloat(f.Name, f.Value);

				for (auto& i : data->Ints)
					ImGui::Extensions::InputInt(i.Name, i.Value);

				for (auto& str : data->Strings)
					ImGui::Extensions::InputRawString(str.Name, str.Value, "Value");

				ImGui::NewLine();
			}
		}
	}

	void EditorLayer::DrawMeshPrimitive(uint32_t type, const std::string& title, Texture* icon)
	{
		ImGui::Image(icon->GetImGuiTexture(), { 60, 60 }, ImVec2(0, 1), ImVec2(1, 0));
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			m_IDBuffer = type;
			ImGui::SetDragDropPayload("MeshPanel", &m_IDBuffer, sizeof(uint32_t));

			ImGui::Image(icon->GetImGuiTexture(), { 40, 40}, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::EndDragDropSource();
		}

		ImGui::SameLine();
		ImGui::TextUnformatted(title.c_str());
	}

	void EditorLayer::CheckActor(Ref<Actor>& actor)
	{
		HeadComponent* head = actor->GetInfo();
		for (auto child : head->Childs)
		{
			if (std::find(m_DisplayedActors.begin(), m_DisplayedActors.end(), child) != m_DisplayedActors.end())
			{
				std::vector<Ref<Actor>> tmp = m_DisplayedActors;
				tmp.erase(std::remove(tmp.begin(), tmp.end(), child), tmp.end());
				m_DisplayedActors = tmp;

				CheckActor(child);
			}
		}
	}

	// TODO: remove hard coded strings
	void EditorLayer::OnFileSelected(const std::string& path, const std::string& ext, int fileSize)
	{
		if (ext == ".s_material")
		{
			m_SelectedActor = nullptr;
			m_SelectionFlags = SelectionFlags::MaterialView;

			if (fileSize > 0)
				m_MaterialInspector->OpenExisting(path);
			else
				m_MaterialInspector->OpenNew(path);
		}

		if (ext == ".s_image")
		{
			m_SelectedActor = nullptr;
			m_SelectionFlags = SelectionFlags::TextureView;
			m_TextureInspector->Open(path);
		}
	}

	void EditorLayer::OnFileDeleted(const std::string& path, const std::string& ext)
	{
		if (ext == ".s_material")
		{
			m_MaterialInspector->Close();
		}

		m_SelectionFlags = SelectionFlags::None;
	}
}