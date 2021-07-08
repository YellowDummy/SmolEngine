#include "stdafx.h"
#include "ViewPort.h"
#include "EditorLayer.h"

#include "ECS/ComponentsCore.h"
#include "ECS/Systems/CameraSystem.h"

#include <Frostium3D/DebugRenderer.h>
#include <Frostium3D/Utils/Utils.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <Frostium3D/Libraries/glm/glm/gtx/quaternion.hpp>
#include <Frostium3D/Libraries/glm/glm/gtx/matrix_decompose.hpp>
#include <Frostium3D/Libraries/glm/glm/gtc/type_ptr.hpp>

namespace SmolEngine
{
	ViewPort::ViewPort()
		:m_World(WorldAdmin::GetSingleton())
	{

	}

	SceneView::SceneView(EditorLayer* editor)
		:m_Editor(editor)
	{
		m_TexturesLoader = TexturesLoader::Get();
	}

	void SceneView::Draw()
	{
		ImGui::Begin("Scene View", nullptr, ImGuiWindowFlags_NoDecoration);
		{
			Ref<Actor>& selectedActor = m_Editor->m_SelectedActor;

			{
				ImGui::NewLine();
				ImGui::SetCursorPosX(10);

				if (ImGui::ImageButton(m_TexturesLoader->m_MoveButton.GetImGuiTexture(), { 25, 25 }))
				{
					m_GizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
				}

				ImGui::SameLine();
				if (ImGui::ImageButton(m_TexturesLoader->m_RotateButton.GetImGuiTexture(), { 25, 25 }))
				{
					m_GizmoOperation = ImGuizmo::OPERATION::ROTATE;
				}


				ImGui::SameLine();
				if (ImGui::ImageButton(m_TexturesLoader->m_ScaleButton.GetImGuiTexture(), { 25, 25 }))
				{
					m_GizmoOperation = ImGuizmo::OPERATION::SCALE;
				}

				ImGui::SameLine();
				ImGui::SetCursorPosX((ImGui::GetWindowWidth() / 2.0f) - 25);
				if (ImGui::ImageButton(m_TexturesLoader->m_PlayButton.GetImGuiTexture(), { 25, 25 }))
				{
					if (!m_World->IsInPlayMode())
					{
						m_World->SaveCurrentScene();
						m_World->OnBeginWorld();
					}
					else
					{
						NATIVE_WARN("The scene is already in play mode!");
					}
				}

				ImGui::SameLine();
				if (ImGui::ImageButton(m_TexturesLoader->m_StopButton.GetImGuiTexture(), { 25, 25 }))
				{
					if (m_World->IsInPlayMode())
					{
						uint32_t selectedActorID = 0;
						if (selectedActor != nullptr)
						{
							selectedActorID = selectedActor->GetID();
						}

						selectedActor = nullptr;
						m_World->OnEndWorld();
						m_World->LoadLastSceneState();
						selectedActor = m_World->GetActiveScene()->FindActorByID(selectedActorID);
					}
					else
					{
						NATIVE_WARN("The scene is not in play mode!");
					}
				}
			}

			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
				ImGui::BeginChild("TetxureScene");
				{
					if (ImGui::IsWindowHovered()) { m_Focused = true; }
					else { m_Focused = false; }

					Framebuffer* fb = Engine::GetEngine()->GetGraphicsContext()->GetFramebuffer();
					ImVec2 ViewPortSize = ImGui::GetContentRegionAvail();
					if (ViewPortSize.x != m_ViewPortSize.x || ViewPortSize.y != m_ViewPortSize.y)
					{
						m_ViewPortSize = { ViewPortSize.x, ViewPortSize.y };
						GraphicsContext::GetSingleton()->SetFramebufferSize(static_cast<uint32_t>(m_ViewPortSize.x), static_cast<uint32_t>(m_ViewPortSize.y));
					}

					ImGui::Image(fb->GetImGuiTextureID(), ImVec2{ m_ViewPortSize.x, m_ViewPortSize.y });
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MeshPanel"))
						{
							std::string name = "DefaultActor" + std::to_string(m_World->GetActiveScene()->GetSceneState()->Actors.size());
							auto actor = m_World->GetActiveScene()->CreateActor(name);
							MeshComponent* meshComp = actor->AddComponent<MeshComponent>();

							uint32_t meshIndex = *(uint32_t*)payload->Data;
							meshComp->eDefaultType = (MeshComponent::DefaultMeshType)meshIndex;
							auto defaultMeshes = GraphicsContext::GetSingleton()->GetDefaultMeshes();

							switch (meshComp->eDefaultType)
							{
							case MeshComponent::DefaultMeshType::Cube: meshComp->DefaulPtr = defaultMeshes->Cube; break;
							case MeshComponent::DefaultMeshType::Sphere: meshComp->DefaulPtr = defaultMeshes->Sphere; break;
							case MeshComponent::DefaultMeshType::Capsule: meshComp->DefaulPtr = defaultMeshes->Capsule; break;
							case MeshComponent::DefaultMeshType::Torus: meshComp->DefaulPtr = defaultMeshes->Torus; break;
							}

							uint32_t size = static_cast<uint32_t>(meshComp->DefaulPtr->GetChilds().size()) + 1;
							meshComp->MaterialsData.resize(size);
							selectedActor = actor;
							m_Editor->m_SelectionFlags = SelectionFlags::Inspector;
						}

						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FileBrowser"))
						{
							std::string path = *(std::string*)payload->Data;

							if (m_Editor->FileExtensionCheck(path, ".s_scene"))
							{
								bool reload = m_World->GetActiveScene()->GetSceneState()->FilePath == path;
								m_World->LoadScene(path, reload);
								selectedActor = nullptr;
							}

							if (m_Editor->FileExtensionCheck(path, ".gltf"))
							{
								std::string name = "DefaultActor" + std::to_string(m_World->GetActiveScene()->GetSceneState()->Actors.size());
								auto actor = m_World->GetActiveScene()->CreateActor(name);
								MeshComponent* mesh = actor->AddComponent<MeshComponent>();
								TransformComponent* transform = actor->GetComponent<TransformComponent>();

								{
									float rayDistance = 20.0f;
									float x = ImGui::GetMousePos().x;
									float y = ImGui::GetMousePos().y - (m_ViewPortSize.y / 2.0f);

									glm::vec3 startPos = m_Editor->m_Camera->GetPosition();
									glm::mat4 viewProj = m_Editor->m_Camera->GetViewProjection();

									transform->WorldPos = Utils::CastRay(startPos, glm::vec2(x, y), m_ViewPortSize.x, m_ViewPortSize.y, rayDistance, viewProj);
								}

								if (mesh)
								{
									ComponentHandler::ValidateMeshComponent(mesh, path);
									selectedActor = actor;
								}
								else
								{
									selectedActor = nullptr;
								}
							}
						}
						ImGui::EndDragDropTarget();
					}

					// Gizmos
					if (selectedActor != nullptr && !m_World->IsInPlayMode())
					{
						auto transformComponent = m_World->GetActiveScene()->GetComponent<TransformComponent>(selectedActor);
						if (transformComponent)
						{
							float snapValue = 0.5f;
							switch (m_Editor->m_Camera->GetType())
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

							if (m_GizmoOperation == ImGuizmo::OPERATION::ROTATE)
								snapValue = 45.0f;

							ImGuizmo::SetDrawlist();
							float width = (float)ImGui::GetWindowSize().x;
							float height = (float)ImGui::GetWindowSize().y;
							ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, width, height);

							glm::mat4 transform;
							glm::mat4 rotation = glm::toMat4(glm::quat(transformComponent->Rotation));
							transform = glm::translate(glm::mat4(1.0f), transformComponent->WorldPos)
								* rotation
								* glm::scale(glm::mat4(1.0f), transformComponent->Scale);

							float snapValues[3] = { snapValue, snapValue, snapValue };

							ImGuizmo::Manipulate(glm::value_ptr(m_Editor->m_Camera->GetViewMatrix()), glm::value_ptr(m_Editor->m_Camera->GetProjection()),
								m_GizmoOperation, ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, m_SnapEnabled ? snapValues : nullptr);

							if (ImGuizmo::IsUsing())
							{
								glm::vec3 tranlation, rotation, scale;
								Utils::DecomposeTransform(transform, tranlation, rotation, scale);

								transformComponent->WorldPos = tranlation;
								transformComponent->Rotation.x = rotation.x;
								transformComponent->Rotation.y = rotation.y;
								transformComponent->Rotation.z = rotation.z;
								transformComponent->Scale = scale;
							}
						}
					}
				}
				ImGui::EndChild();
				ImGui::PopStyleVar();
			}

		}
		ImGui::End();
	}

	GameView::GameView()
	{
		FramebufferSpecification framebufferCI = {};
		const WindowData* windowData = GraphicsContext::GetSingleton()->GetWindowData();

		framebufferCI.Width = windowData->Width;
		framebufferCI.Height = windowData->Width;
		framebufferCI.eMSAASampels = MSAASamples::SAMPLE_COUNT_1;
		framebufferCI.bTargetsSwapchain = false;
		framebufferCI.bResizable = true;
		framebufferCI.bAutoSync = false;
		framebufferCI.bUsedByImGui = true;
		framebufferCI.Attachments = { FramebufferAttachment(AttachmentFormat::Color) };

		Framebuffer::Create(framebufferCI, &m_PreviewFramebuffer);
	}

	void GameView::Draw()
	{
		if (m_Active)
		{
			ImGui::Begin("Game View", &m_Active);
			{
				if (ImGui::IsWindowHovered()) { m_Focused = true; }
				else { m_Focused = false; }

				Framebuffer* fb = &m_PreviewFramebuffer;
				ImVec2 ViewPortSize = ImGui::GetContentRegionAvail();
				if (ViewPortSize.x != m_ViewPortSize.x || ViewPortSize.y != m_ViewPortSize.y)
				{
					m_ViewPortSize = { ViewPortSize.x, ViewPortSize.y };
					m_PreviewFramebuffer.OnResize(static_cast<uint32_t>(m_ViewPortSize.x), static_cast<uint32_t>(m_ViewPortSize.y));
				}

				ImGui::Image(fb->GetImGuiTextureID(), ImVec2{ m_ViewPortSize.x, m_ViewPortSize.y});
			}

			ImGui::End();
		}
	}

	void GameView::Render()
	{
		auto& reg = m_World->GetActiveScene()->GetRegistry();
		const auto& cameraGroup = reg.view<CameraComponent, TransformComponent>();
		for (const auto& entity : cameraGroup)
		{
			const auto& [camera, transform] = cameraGroup.get<CameraComponent, TransformComponent>(entity);
			if (camera.bPrimaryCamera == false) { continue; }

			// Calculating ViewProj
			CameraSystem::CalculateView(&camera, &transform);

			BeginSceneInfo sceneInfo = {};
			sceneInfo.View = camera.ViewMatrix;
			sceneInfo.Proj = camera.ProjectionMatrix;
			sceneInfo.NearClip = camera.zNear;
			sceneInfo.FarClip = camera.zFar;
			sceneInfo.Pos = transform.WorldPos;

			DeferredRenderer::DrawOffscreen(&m_PreviewFramebuffer, &sceneInfo);
			break;
		}
	}
}