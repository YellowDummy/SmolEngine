#include "stdafx.h"
#include "EditorLayer.h"
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

#include <Frostium3D/ImGUI/ImGuiExtension.h>
#include <Frostium3D/Utils/Utils.h>
#include <Frostium3D/Common/Input.h>
#include <Frostium3D/Renderer2D.h>
#include <Frostium3D/Renderer.h>

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

using namespace Frostium;

namespace SmolEngine
{
	static bool showConsole = true;
	static bool showGameView = false;
	static bool showMaterialLibrary = false;

	void EditorLayer::LoadAssets()
	{
		const bool flip = true;
		const bool imguiDescriptor = true;

		Frostium::Texture::Create("assets/buttons/play_button.png", &m_PlayButton, TextureFormat::R8G8B8A8_UNORM, flip, imguiDescriptor);
		Frostium::Texture::Create("assets/buttons/pause_button.png", &m_StopButton, TextureFormat::R8G8B8A8_UNORM, flip, imguiDescriptor);
		Frostium::Texture::Create("assets/buttons/move_button.png", &m_MoveButton, TextureFormat::R8G8B8A8_UNORM, flip, imguiDescriptor);
		Frostium::Texture::Create("assets/buttons/rotate_button.png", &m_RotateButton, TextureFormat::R8G8B8A8_UNORM, flip, imguiDescriptor);
		Frostium::Texture::Create("assets/buttons/scale_button.png", &m_ScaleButton, TextureFormat::R8G8B8A8_UNORM, flip, imguiDescriptor);
		Frostium::Texture::Create("assets/buttons/search_button.png", &m_SearchButton, TextureFormat::R8G8B8A8_UNORM, flip, imguiDescriptor);
		Frostium::Texture::Create("assets/buttons/remove_button.png", &m_RemoveButton, TextureFormat::R8G8B8A8_UNORM, flip, imguiDescriptor);
		Frostium::Texture::Create("assets/buttons/folder_button.png", &m_FolderButton, TextureFormat::R8G8B8A8_UNORM, flip, imguiDescriptor);
	}

	void EditorLayer::OnAttach()
	{
		LoadAssets();

		m_MaterialLibraryInterface = new MaterialLibraryInterface(this);
		m_Console = new EditorConsole();
		m_FileManager = new FileManager();
		m_FileManager->Init();
		m_World = WorldAdmin::GetSingleton();
		m_World->CreateScene(std::string("TestScene2.s_scene"));

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
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnBeginFrame(Frostium::DeltaTime deltaTime)
	{
		if (m_IsSceneViewFocused)
		{
			m_Camera->OnUpdate(deltaTime);
		}

		BeginSceneInfo bSeneInfo = {};
		bSeneInfo.Update(m_Camera);
		m_World->SetBeginSceneInfo(&bSeneInfo);
	}

	void EditorLayer::OnUpdate(Frostium::DeltaTime deltaTime)
	{

	}

	void EditorLayer::OnEvent(Frostium::Event& e)
	{
		if(m_IsSceneViewFocused)
			m_Camera->OnEvent(e);
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

		DrawToolsBar();
		DrawSceneView(true);

		m_Console->Update(showConsole);
		m_FileManager->Update();
		m_MaterialLibraryInterface->Update(showMaterialLibrary);

		DrawHierarchy();
		DrawInspector();

		ImGui::End();

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
						const auto& result = Frostium::Utils::SaveFile("SmolEngine Scene (*.s_scene)\0*.s_scene\0", "new_scene.s_scene");
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
							CONSOLE_ERROR("Couldn't save current scene!");
						}
					}

					if (ImGui::MenuItem("Save as"))
					{
						const auto& result = Frostium::Utils::SaveFile("SmolEngine Scene (*.s_scene)\0*.s_scene\0", "new_scene.s_scene");
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
						const auto& result = Frostium::Utils::OpenFile("SmolEngine Scene (*.s_scene)\0*.s_scene\0");
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
				if (ImGui::MenuItem("Material Library"))
				{
					showMaterialLibrary = true;
				}

				if (ImGui::MenuItem("Console"))
				{
					showConsole = true;
				}

				if (ImGui::MenuItem("Content Broser"))
				{
					m_FileManager->Open();
				}

				ImGui::EndMenu();
			}

		}
		ImGui::EndMainMenuBar();
		ImGui::PopStyleVar();

	}

	void EditorLayer::DrawSceneTetxure()
	{
		const float snapValue = 0.5f;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
		ImGui::BeginChild("TetxureScene");
		{

			if (ImGui::IsWindowHovered()) { m_IsSceneViewFocused = true; }
			else { m_IsSceneViewFocused = false; }

			Frostium::Framebuffer* fb = Engine::GetEngine()->GetGraphicsContext()->GetFramebuffer();
			m_SceneViewSize = { ImGui::GetWindowSize().x, ImGui::GetWindowSize().y };
			ImVec2 ViewPortSize = ImGui::GetContentRegionAvail();
			if (ViewPortSize.x != m_ViewPortSize.x || ViewPortSize.y != m_ViewPortSize.y)
			{
				m_ViewPortSize = { ViewPortSize.x, ViewPortSize.y };
			}

#ifdef SMOLENGINE_OPENGL_IMPL
			//ImGui::Image(frameBuffer->GetImGuiTextureID(), ImVec2{ m_ViewPortSize.x, m_ViewPortSize.y }, ImVec2(0, 1), ImVec2(1, 0));
#else
			ImGui::Image(fb->GetImGuiTextureID(), ImVec2{ m_ViewPortSize.x, m_ViewPortSize.y });
#endif
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FileBrowser"))
				{
					std::string path;
					std::filesystem::path* p = (std::filesystem::path*)payload->Data;
					if (FileExtensionCheck(p, ".s_scene", path))
						m_World->LoadScene(path);
				}
				ImGui::EndDragDropTarget();
			}

			// Gizmos
			if (m_SelectedActor != nullptr && !m_World->IsInPlayMode())
			{
				auto transformComponent = m_World->GetActiveScene()->GetComponent<TransformComponent>(m_SelectedActor);
				if (transformComponent)
				{
					switch (m_Camera->GetType())
					{
					case CameraType::Perspective:
					{
						ImGuizmo::SetOrthographic(false);
						break;
					}
					case CameraType::Ortho:
					{
						ImGuizmo::SetOrthographic(true);
						break;
					}
					default:
						break;
					}

					ImGuizmo::SetDrawlist();

					float width = (float)ImGui::GetWindowSize().x;
					float height = (float)ImGui::GetWindowSize().y;

					ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, width, height);

					glm::mat4 transform;

					Utils::ComposeTransform(transformComponent->WorldPos, transformComponent->Rotation, transformComponent->Scale, transform);
					float snapValues[3] = { snapValue, snapValue, snapValue };

					ImGuizmo::Manipulate(glm::value_ptr(m_Camera->GetViewMatrix()), glm::value_ptr(m_Camera->GetProjection()),
						m_GizmoOperation, ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, snapValues);

					if (ImGuizmo::IsUsing())
					{
						glm::vec3 tranlation, rotation, scale;
						Utils::DecomposeTransform(transform, tranlation, rotation, scale);
						glm::vec3 deltaRot = rotation - transformComponent->Rotation;

						transformComponent->WorldPos = tranlation;
						transformComponent->Rotation.x += deltaRot.x;
						transformComponent->Rotation.y += deltaRot.y;
						transformComponent->Rotation.z += deltaRot.z;
						transformComponent->Scale = scale;
					}
				}
			}
		}
		ImGui::EndChild();
		ImGui::PopStyleVar();
	}

	void EditorLayer::DrawInfo(HeadComponent* head)
	{
		m_TempActorName = head->Name;
		m_TempActorTag = head->Tag;

		if (ImGui::Extensions::InputRawString("Name", m_TempActorName))
			m_SelectedActor->SetName(m_TempActorName);

		ImGui::Extensions::InputString("Tag", head->Tag, m_TempActorTag);
		ImGui::Extensions::CheckBox("Enabled", head->IsEnabled, 130.0f, "HeadPanel");
	}

	void EditorLayer::DrawTransform(TransformComponent* transform)
	{
		ImGui::Extensions::TransformComponent(transform->WorldPos, transform->Scale, transform->Rotation);
	}

	void EditorLayer::DrawTexture(Texture2DComponent* texture)
	{
		if (texture->Texture != nullptr)
		{
			ImGui::Extensions::ColorInput3("Color", texture->Color);
			ImGui::Extensions::InputInt("Layer", texture->LayerIndex, 130.0f, "TexturePanel");
			ImGui::NewLine();
			ImGui::Extensions::CheckBox("Enabled", texture->Enabled, 130.0f, "TexturePanel");
		}

		ImGui::NewLine();
		ImGui::SetCursorPosX(10);
		if (ImGui::Button("Load Texture", { ImGui::GetWindowWidth() - 20.0f, 30.0f }))
		{
			const auto& result = Frostium::Utils::OpenFile("png (*png)\0*.png\0jpg (*jpg)\0*.jpg\0");
			if (result.has_value())
				ComponentHandler::ValidateTexture2DComponent(texture, result.value());
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FileBrowser"))
			{
				std::string path;
				std::filesystem::path* p = (std::filesystem::path*)payload->Data;

				if (FileExtensionCheck(p, ".png", path))
					ComponentHandler::ValidateTexture2DComponent(texture, path);

				if (FileExtensionCheck(p, ".jpg", path))
					ComponentHandler::ValidateTexture2DComponent(texture, path);
			}
			ImGui::EndDragDropTarget();
		}

	}

	void EditorLayer::DrawBody2D(Body2DComponent* rb)
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
		ImGui::Extensions::InputFloat("Zoom", camera->ZoomLevel);
		ImGui::Extensions::InputFloat("Near", camera->zNear);
		ImGui::Extensions::InputFloat("Far", camera->zFar);

		ImGui::NewLine();

		if(ImGui::Extensions::CheckBox("Primary", camera->isPrimaryCamera))
		{
			if (camera->isPrimaryCamera)
			{
				const size_t id = m_SelectedActor->GetID();

				entt::registry& reg = m_World->GetActiveScene()->GetRegistry();
				reg.view<HeadComponent, CameraComponent>().each([&](HeadComponent& head, CameraComponent& camera)
				{
					if (head.ActorID != id)
						camera.isPrimaryCamera = false;
				});
			}
		}

		ImGui::Extensions::CheckBox("Enabled", camera->isEnabled);
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
		ImGui::Extensions::ColorInput3("Color", light->Color, 130.0f, "2DLightPanel");

		ImGui::NewLine();

		ImGui::Extensions::CheckBox("Enabled", light->IsEnabled, 130.0f, "2DLightPanel");
	}

	void EditorLayer::DrawSceneView(bool enabled)
	{
		if (enabled)
		{
			ImGui::Begin("Scene View", nullptr, ImGuiWindowFlags_NoDecoration);
			{
				ImGui::NewLine();
				ImGui::SetCursorPosX(10);
				if (ImGui::ImageButton(m_MoveButton.GetImGuiTexture(), { 25, 25 }))
				{
					m_GizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
				}

				ImGui::SameLine();
				if (ImGui::ImageButton(m_RotateButton.GetImGuiTexture(), { 25, 25 }))
				{
					m_GizmoOperation = ImGuizmo::OPERATION::ROTATE;
				}


				ImGui::SameLine();
				if (ImGui::ImageButton(m_ScaleButton.GetImGuiTexture(), { 25, 25 }))
				{
					m_GizmoOperation = ImGuizmo::OPERATION::SCALE;
				}

				ImGui::SameLine();
				ImGui::SetCursorPosX((ImGui::GetWindowWidth() / 2.0f) - 25);
				if (ImGui::ImageButton(m_PlayButton.GetImGuiTexture(), { 25, 25 }))
				{
					if (!m_World->IsInPlayMode())
					{
						m_World->OnBeginWorld();
					}
					else
					{
						CONSOLE_WARN("The scene is already in play mode!");
					}
				}

				ImGui::SameLine();
				if (ImGui::ImageButton(m_StopButton.GetImGuiTexture(), { 25, 25 }))
				{
					if (m_World->IsInPlayMode())
					{
						uint32_t selectedActorID = 0;
						if (m_SelectedActor != nullptr)
						{
							selectedActorID = m_SelectedActor->GetID();
						}

						m_SelectedActor = nullptr;
						m_World->OnEndWorld();
						m_SelectedActor = m_World->GetActiveScene()->FindActorByID(selectedActorID);
					}
					else
					{
						CONSOLE_WARN("The scene is not in play mode!");
					}
				}

				DrawSceneTetxure();
			}
			ImGui::End();
		}
	}

	void EditorLayer::DrawInspector()
	{
		ImGui::Begin("Inspector");
		{
			ImGui::BeginChild("InspectorChild");
			{
				ImGui::SetWindowFontScale(0.9f);
				if (m_SelectedActor == nullptr || m_SelectionFlags != SelectionFlags::Inspector)
				{
					ImGui::Text("No Actor selected");
					ImGui::EndChild();
					ImGui::End();
					return;
				}
				else
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
			ImGui::SetWindowFontScale(0.9f);
			ImGui::Image(m_SearchButton.GetImGuiTexture(), { 25, 25 }, ImVec2(0, 1), ImVec2(1, 0));
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
					if (Frostium::Input::IsMouseButtonPressed(Frostium::MouseCode::Button1))
					{
						ImGui::OpenPopup("CreateActorPopUp");
					}

					if (Frostium::Input::IsMouseButtonPressed(Frostium::MouseCode::Button0))
						m_SelectedActor = nullptr;
				}

				if (ImGui::BeginPopup("CreateActorPopUp"))
				{
					ImGui::MenuItem("New Actor", NULL, false, false);
					ImGui::Separator();
					std::stringstream ss;

					if (ImGui::MenuItem("Empty Actor"))
					{
						ss << "EmptyActor" << state->Actors.size();
						m_SelectedActor = m_World->GetActiveScene()->CreateActor(ss.str());
					}

					ImGui::EndPopup();
				}

				if (ImGui::TreeNodeEx(sceneStr.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				{
					std::vector<Actor*> actors;
					m_World->GetActiveScene()->GetActorsByID(actors);
					for (const auto& actor : actors)
					{
						auto result = actor->GetName().find(name);
						if (result == std::string::npos)
						{
							continue;
						}

						if (ImGui::TreeNodeEx(actor->GetName().c_str(), ImGuiTreeNodeFlags_OpenOnArrow))
						{
							ImGui::TreePop();
						}


						if (ImGui::IsItemClicked(1))
						{
							m_SelectionFlags = SelectionFlags::Actions;

							m_TempActorTag = "";
							m_TempActorName = "";

							m_SelectedActor = nullptr;
							m_SelectedActor = actor;
						}

						if (ImGui::IsItemClicked())
						{
							m_SelectionFlags = SelectionFlags::Inspector;
							if (m_SelectedActor != nullptr)
							{
								m_SelectedActor->GetInfo()->ShowComponentUI = false;
								m_SelectedActor = nullptr;
							}

							m_TempActorTag = "";
							m_TempActorName = "";

							m_SelectedActor = actor;
						}
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
		static bool showMeshInspector = false;
		if (comp->Mesh)
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
			const auto& result = Frostium::Utils::OpenFile("glTF 2.0 (*gltf)\0*.gltf\0");
			if (result.has_value())
			{
				ComponentHandler::ValidateMeshComponent(comp, result.value());
			}
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FileBrowser"))
			{
				std::string path;
				std::filesystem::path* p = (std::filesystem::path*)payload->Data;
				if (FileExtensionCheck(p, ".gltf", path))
					ComponentHandler::ValidateMeshComponent(comp, path);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::NewLine();
	}

	void EditorLayer::DrawDirectionalLightComponent(DirectionalLightComponent* comp)
	{
		glm::vec3* dir = (glm::vec3*)&comp->Light.Direction;
		bool* cast_shadow = (bool*)&comp->Light.IsCastShadows;
		bool* is_active = (bool*)&comp->Light.IsActive;

		ImGui::Extensions::CheckBox("Use", *is_active);
		ImGui::Extensions::CheckBox("Cast Shadows", *cast_shadow);

		ImGui::Extensions::InputFloat("Intensity", comp->Light.Intensity);
		ImGui::Extensions::DragFloat3Base("Direction", *dir);
		ImGui::Extensions::ColorInput3("Color", comp->Light.Color);
	}

	void EditorLayer::DrawMeshInspector(bool& show)
	{
		auto* comp = m_World->GetActiveScene()->GetComponent<MeshComponent>(m_SelectedActor);
		if (!comp)
			return;

		if (show)
		{
			uint32_t count = static_cast<uint32_t>(comp->Mesh->GetChildCount());
			std::vector<Frostium::Mesh*> meshes(count + 1);
			for (uint32_t i = 0; i < count + 1; ++i)
			{
				if (i == 0)
				{
					meshes[0] = comp->Mesh.get();
					continue;
				}

				Frostium::Mesh* mesh = &comp->Mesh->GetChilds()[i - 1];
				meshes[i] = mesh;
			}

			ImGui::Begin("Mesh Inspector", &show);
			{
				ImGui::Extensions::Text("Mesh & SubMeshes", "");

				for (uint32_t i = 0; i < static_cast<uint32_t>(meshes.size()); ++i)
				{
					Frostium::Mesh* mesh = meshes[i];

					std::string name = "Mesh #" + std::to_string(i);
					if (ImGui::CollapsingHeader(name.c_str()))
					{
						std::string id = name + "IDMat";
						ImGui::PushID(id.c_str());
						{
							ImGui::Extensions::Text("Material ID", std::to_string(mesh->GetMaterialID()));
							if (ImGui::Button("Select Material"))
							{
								const auto& result = Frostium::Utils::OpenFile("SmolEngine Material (*s_material)\0*.s_material\0");
								if (result.has_value())
								{
									ComponentHandler::SetMeshMaterial(comp, mesh, result.value());
								}
							}

							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FileBrowser"))
								{
									std::string path;
									std::filesystem::path* p = (std::filesystem::path*)payload->Data;
									if (FileExtensionCheck(p, ".s_material", path))
										ComponentHandler::SetMeshMaterial(comp, mesh, path);
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
		bool* cast_shadow = (bool*)&comp->Light.IsCastShadows;
		bool* is_active = (bool*)&comp->Light.IsActive;

		ImGui::Extensions::CheckBox("Enabled", *is_active);
		ImGui::Extensions::InputFloat("Exposure", comp->Light.Intensity);
		ImGui::Extensions::InputFloat("Radius", comp->Light.Raduis);
		ImGui::Extensions::ColorInput3("Color", comp->Light.Color);
	}

	void EditorLayer::DrawRigidBodyComponent(RigidbodyComponent* component)
	{
		ImGui::Extensions::Combo("Type", "Dynamic\0Static\0", component->CreateInfo.StateIndex);
		ImGui::Extensions::Combo("Shape", "Sphere\0Capsule\0Box\0Custom\0", component->CreateInfo.ShapeIndex);

		component->CreateInfo.eShape = (RigidBodyShape)component->CreateInfo.ShapeIndex;

		if (component->CreateInfo.ShapeIndex == 0)
		{
			ImGui::Extensions::InputFloat("Radius", component->CreateInfo.SphereShape.Radius);
		}

		if (component->CreateInfo.ShapeIndex == 1)
		{
			ImGui::Extensions::InputFloat("Radius", component->CreateInfo.CapsuleShapeInfo.Radius);
			ImGui::Extensions::InputFloat("Height", component->CreateInfo.CapsuleShapeInfo.Height);
		}

		if (component->CreateInfo.ShapeIndex == 2)
		{
			ImGui::Extensions::InputFloat("Side X", component->CreateInfo.BoxShapeInfo.X);
			ImGui::Extensions::InputFloat("Side Y", component->CreateInfo.BoxShapeInfo.Y);
			ImGui::Extensions::InputFloat("Side Z", component->CreateInfo.BoxShapeInfo.Z);
		}

		ImGui::Separator();
		ImGui::NewLine();

		ImGui::Extensions::InputFloat("Mass", component->CreateInfo.Mass);
		ImGui::Extensions::InputFloat("Density", component->CreateInfo.Density);
		ImGui::Extensions::InputFloat("Friction", component->CreateInfo.Friction);
		ImGui::Extensions::InputFloat("Restitution", component->CreateInfo.Restitution);
		ImGui::Extensions::InputFloat("Linear Damping", component->CreateInfo.LinearDamping);
		ImGui::Extensions::InputFloat("Angular Damping", component->CreateInfo.AngularDamping);
		ImGui::Extensions::InputFloat("Rolling Friction", component->CreateInfo.RollingFriction);
		ImGui::Extensions::InputFloat("Spinning Friction", component->CreateInfo.SpinningFriction);
		ImGui::Extensions::InputFloat3Base("Local Inertia", component->CreateInfo.LocalInertia);
	}

	void EditorLayer::DrawComponents()
	{
		// Head
		if (ImGui::CollapsingHeader("Head"))
		{
			ImGui::NewLine();
			auto info = m_World->GetActiveScene()->GetComponent<HeadComponent>(m_SelectedActor);
			DrawInfo(info);
		}

		// Transform 

		if (ImGui::CollapsingHeader("Tranform"))
		{
			ImGui::NewLine();
			DrawTransform(m_World->GetActiveScene()->GetComponent<TransformComponent>(m_SelectedActor));
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

			if (IsCurrentComponent<Body2DComponent>(i))
			{
				if (ImGui::CollapsingHeader("Body 2D"))
				{
					ImGui::NewLine();
					auto component = m_World->GetActiveScene()->GetComponent<Body2DComponent>(m_SelectedActor);
					DrawBody2D(component);
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

			if (IsCurrentComponent<DirectionalLightComponent>(i))
			{
				if (ImGui::CollapsingHeader("Directional Light"))
				{
					ImGui::NewLine();
					auto component = m_World->GetActiveScene()->GetComponent<DirectionalLightComponent>(m_SelectedActor);
					DrawDirectionalLightComponent(component);
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
			ImGui::MenuItem("New Component", NULL, false, false);
			ImGui::Separator();

			if (ImGui::BeginMenu("Base"))
			{
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
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Light"))
			{
				if (ImGui::MenuItem("Point Light 2D"))
				{
					m_World->GetActiveScene()->AddComponent<Light2DSourceComponent>(m_SelectedActor);
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Point Light"))
				{
					m_World->GetActiveScene()->AddComponent<PointLightComponent>(m_SelectedActor);
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Directional Light"))
				{
					m_World->GetActiveScene()->AddComponent<DirectionalLightComponent>(m_SelectedActor);
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Physics"))
			{
				if (ImGui::MenuItem("Body 2D"))
				{
					auto comp = m_World->GetActiveScene()->AddComponent<Body2DComponent>(m_SelectedActor);
					ComponentHandler::ValidateBody2DComponent(comp, m_SelectedActor);
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("RigidBody"))
				{
					auto comp = m_World->GetActiveScene()->AddComponent<RigidbodyComponent>(m_SelectedActor);
					ComponentHandler::ValidateRigidBodyComponent(comp, m_SelectedActor);
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Common"))
			{

				if (ImGui::MenuItem("Animation 2D"))
				{
					m_World->GetActiveScene()->AddComponent<Animation2DComponent>(m_SelectedActor);
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Audio Source"))
				{
					m_World->GetActiveScene()->AddComponent<AudioSourceComponent>(m_SelectedActor);
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

				ImGui::EndMenu();
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

	bool EditorLayer::FileExtensionCheck(std::filesystem::path* path, const std::string& name, std::string& strPath)
	{
		if (path->extension().filename() == name)
		{
			strPath = path->u8string();
			return true;
		}

		return false;
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
}