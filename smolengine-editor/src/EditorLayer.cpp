#include "stdafx.h"
#include "EditorLayer.h"

#include "Core/MaterialLibraryInterface.h"
#include "Core/FileDialog.h"
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
#define GLM_ENABLE_EXPERIMENTAL
#include <Frostium3D/Libraries/glm/glm/gtx/quaternion.hpp>
#include <Frostium3D/Libraries/glm/glm/gtx/matrix_decompose.hpp>
#include <Frostium3D/Libraries/glm/glm/gtc/type_ptr.hpp>
#include <Libraries/entt/entt.hpp>


#include <fstream>

using namespace Frostium;

namespace SmolEngine
{
	void EditorLayer::OnAttach()
	{
		m_MaterialLibraryInterface = std::make_unique<MaterialLibraryInterface>();

		m_Console = new EditorConsole();
		m_World = WorldAdmin::GetSingleton();
		m_World->CreateScene(std::string("TestScene2.s_scene"));
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
		m_Camera->OnEvent(e);
	}

	void EditorLayer::OnImGuiRender()
	{
		//---------------------------------------WINDOW-STATES----------------------------------------//

		static bool showRenderer2Dstats;
		static bool showConsole = true;
		static bool showGameView = false;
		static bool showMaterialLibrary = false;

		//---------------------------------------WINDOW-STATES----------------------------------------//

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
						const auto& result = FileDialog::SaveFile("SmolEngine Scene (*.s_scene)\0*.s_scene\0", "new_scene.s_scene");
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
						const auto& result = FileDialog::SaveFile("SmolEngine Scene (*.s_scene)\0*.s_scene\0", "new_scene.s_scene");
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
						const auto& result = FileDialog::OpenFile("SmolEngine Scene (*.s_scene)\0*.s_scene\0");
						if (result.has_value())
						{
							m_SelectedActor = nullptr;
							m_World->LoadScene(result.value());
						}
					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Simulation"))
			{
				if (ImGui::MenuItem("Play Mode"))
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

				if (ImGui::MenuItem("Stop"))
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

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Material Library"))
				{
					showMaterialLibrary = true;
				}

				if (ImGui::MenuItem("Renderer2D Stats"))
				{
					showRenderer2Dstats = true;
				}

				if (ImGui::MenuItem("Console"))
				{
					showConsole = true;
				}

				ImGui::EndMenu();
			}

		}
		ImGui::EndMainMenuBar();
		ImGui::PopStyleVar();

		m_Console->Update(showConsole);
		DrawSceneView(true);
		m_MaterialLibraryInterface->Draw(showMaterialLibrary);

		DrawHierarchy();
		DrawInspector();
		ImGui::End();

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
			if (ImGui::Extensions::CheckBox("Enabled?", texture->Enabled, 130.0f, "TexturePanel"))

			ImGui::NewLine();
			ImGui::NewLine();
			if (ImGui::Button("Change", { ImGui::GetWindowWidth() - 20.0f, 30.0f }))
			{
				const auto& result = FileDialog::OpenFile("png (*png)\0*.png\0jpg (*jpg)\0*.jpg\0");
				if (result.has_value())
					ComponentHandler::ValidateTexture2DComponent(texture, result.value());
			}

			return;
		}


		if (ImGui::Button("New texture", { ImGui::GetWindowWidth() - 20.0f, 30.0f }))
		{
			const auto& result = FileDialog::OpenFile("png (*png)\0*.png\0jpg (*jpg)\0*.jpg\0");
			if (result.has_value())
				ComponentHandler::ValidateTexture2DComponent(texture, result.value());
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

		ImGui::Extensions::CheckBox("Is Enabled?", camera->isEnabled);
		ImGui::Extensions::CheckBox("Show Shape?", camera->ShowCanvasShape);
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
				ImGui::Extensions::CheckBox("Default Clip?", clip->isDefaultClip);
				ImGui::Extensions::CheckBox("Is Looping?", clip->IsLooping);
				ImGui::Extensions::CheckBox("Is 3D?", clip->B3D);

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
		bool snap = false;
		float snapValue = 0.5f;
		if (Input::IsKeyPressed(KeyCode::LeftShift))
		{
			snap = true;
		}
		if (Input::IsKeyPressed(KeyCode::Z))
		{
			m_GizmoOperation = ImGuizmo::OPERATION::SCALE;
		}
		if (Input::IsKeyPressed(KeyCode::Q))
		{
			m_GizmoOperation = ImGuizmo::OPERATION::ROTATE;
			snapValue = 45.0f;
		}
		if (Input::IsKeyPressed(KeyCode::E))
		{
			m_GizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		}

		if (enabled)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
			ImGui::Begin("Scene View", &enabled);
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
							m_GizmoOperation, ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);

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
			ImGui::End();
			ImGui::PopStyleVar();
		}
	}

	void EditorLayer::DrawInspector()
	{
		ImGui::Begin("Inspector");
		{
			ImGui::BeginChild("InspectorChild");

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


				if (ImGui::BeginPopup("AddCScriptPopUp"))
				{
					ImGui::MenuItem("New Script", NULL, false, false);
					ImGui::Separator();
					ImGui::EndPopup();
				}

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
						if (ImGui::MenuItem("Rigidbody 2D"))
						{
							m_World->GetActiveScene()->AddComponent<Body2DComponent>(m_SelectedActor, m_SelectedActor, 0);
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
						if (ImGui::CollapsingHeader("Rigidbody 2D"))
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

					DrawScriptComponent(i);
				}

				ImGui::Separator();
				ImGui::NewLine();
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
			static char name[128];
			ImGui::InputTextWithHint("Search", "Name", name, IM_ARRAYSIZE(name));
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
						m_World->GetActiveScene()->CreateActor(ss.str());
					}

					if (ImGui::BeginMenu("Lights"))
					{
						if (ImGui::MenuItem("Point Light 2D"))
						{
							ss << "Light2D" << state->Actors.size();
							m_World->GetActiveScene()->AddComponent<Light2DSourceComponent>
								(m_World->GetActiveScene()->CreateActor(ss.str()));
						}

						if (ImGui::MenuItem("Point Light"))
						{
							ss << "PointLight" << state->Actors.size();
							m_World->GetActiveScene()->AddComponent<PointLightComponent>
								(m_World->GetActiveScene()->CreateActor(ss.str()));
						}

						if (ImGui::MenuItem("Directional Light"))
						{
							ss << "DirectionalLight" << state->Actors.size();
							m_World->GetActiveScene()->AddComponent<DirectionalLightComponent>
								(m_World->GetActiveScene()->CreateActor(ss.str()));
						}

						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("Physics"))
					{
						if (ImGui::MenuItem("Rigidbody 2D"))
						{
							ss << "Rigidbody2D" << state->Actors.size();
							Actor* actor = m_World->GetActiveScene()->CreateActor(ss.str());
							m_World->GetActiveScene()->AddComponent<Body2DComponent>(actor,
								actor, 0);
						}

						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("2D"))
					{
						if (ImGui::MenuItem("Sprite"))
						{
							ss << "Sprite" << state->Actors.size();
							m_World->GetActiveScene()->AddComponent<Texture2DComponent>
								(m_World->GetActiveScene()->CreateActor(ss.str()));
						}

						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("3D"))
					{
						if (ImGui::MenuItem("Mesh"))
						{
							ss << "Mesh" << state->Actors.size();
							m_World->GetActiveScene()->AddComponent<MeshComponent>
								(m_World->GetActiveScene()->CreateActor(ss.str()));
						}


						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("Common"))
					{
						if (ImGui::MenuItem("Audio Source"))
						{
							ss << "AudioSource" << state->Actors.size();
							m_World->GetActiveScene()->AddComponent<AudioSourceComponent>
								(m_World->GetActiveScene()->CreateActor(ss.str()));
						}

						if (ImGui::MenuItem("Animation 2D"))
						{
							ss << "Animation2D" << state->Actors.size();
							m_World->GetActiveScene()->AddComponent<Animation2DComponent>
								(m_World->GetActiveScene()->CreateActor(ss.str()));
						}

						if (ImGui::MenuItem("Canvas"))
						{
							ss << "Canvas" << state->Actors.size();
							m_World->GetActiveScene()->AddComponent<CanvasComponent>
								(m_World->GetActiveScene()->CreateActor(ss.str()));
						}

						if (ImGui::MenuItem("Camera"))
						{
							ss << "Camera" << state->Actors.size();
							m_World->GetActiveScene()->AddComponent<CameraComponent>
								(m_World->GetActiveScene()->CreateActor(ss.str()));
						}

						ImGui::EndMenu();
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
			ImGui::NewLine();

			ImGui::Extensions::CheckBox("Show", comp->bShow);
			ImGui::Extensions::CheckBox("Static", comp->bIsStatic);
			ImGui::Extensions::CheckBox("Cast Shadows", comp->bIsStatic);

			ImGui::NewLine();

			if (!showMeshInspector)
			{
				if (ImGui::Button("Mesh Inspector", { ImGui::GetWindowWidth() - 20.0f, 30.0f }))
				{
					showMeshInspector = true;
				}
			}

			DrawMeshInspector(showMeshInspector);
		}

		if (ImGui::Button("Load Mesh", { ImGui::GetWindowWidth() - 20.0f, 30.0f }))
		{
			const auto& result = FileDialog::OpenFile("glTF 2.0 (*gltf)\0*.gltf\0");
			if (result.has_value())
			{
				ComponentHandler::ValidateMeshComponent(comp, result.value());
			}
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
								const auto& result = FileDialog::OpenFile("SmolEngine Material (*s_mat)\0*.s_mat\0");
								if (result.has_value())
								{
									ComponentHandler::SetMeshMaterial(comp, mesh, result.value());
								}
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