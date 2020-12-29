#include "stdafx.h"
#include "AnimationPanel.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <../../imgui/misc/cpp/imgui_stdlib.h>
#include <../../imgui/imgui_internal.h>

#include "Core/SLog.h"
#include "Core/Application.h"

#include "Core/Animation/AnimationClip.h"

#include "Core/Renderer/Renderer2D.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/Camera.h"
#include "Core/Renderer/Texture.h"

#include "Core/ImGui/EditorConsole.h"

#include "Core/ECS/Systems/Animation2DSystem.h"
#include "Core/AssetManager.h"

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>


namespace SmolEngine
{
	AnimationPanel::AnimationPanel()
	{
		m_FileBrowser = std::make_unique<ImGui::FileBrowser>();

		float ar = 280.0f / 280.0f;
		m_Camera = std::make_unique<CameraController>(ar);
		m_Camera->SetZoom(1.0f);
	}

	void AnimationPanel::Update(bool& isOpened)
	{
		if (isOpened && m_AnimationClip != nullptr)
		{
			static bool showPreview = false;
			static bool showTools = false;

			ImGui::Begin("Animation Clip Editor", &isOpened, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking |ImGuiWindowFlags_MenuBar);
			{
				ImGui::SetWindowSize("Animation Clip Editor", { 780, 530 });

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 10.0f, 10.0f });
				if (ImGui::BeginMenuBar())
				{

					if (ImGui::MenuItem("New Frame"))
					{
						m_FileBrowser = std::make_unique<ImGui::FileBrowser>(ImGuiFileBrowserFlags_MultipleSelection);

						m_FileBrowser->SetTitle("Select a sprite");
						m_FileBrowser->SetTypeFilters({ ".png" });
						m_FileBrowser->Open();

						m_SelectionState = AnimationPanelSelectionState::NewFrame;
					}

					if (ImGui::MenuItem("Clear"))
					{
						Animation2DSystem::DebugResetAllFrames(m_AnimationClip.get());
					}

					if (ImGui::MenuItem("Preview"))
					{
						showPreview = true;
						if (m_AnimationClip->m_IsActive)
						{
							m_AnimationClip->m_IsActive = false;
						}

						Animation2DSystem::DebugPlay(m_AnimationClip.get());
					}

					if (ImGui::MenuItem("Save As"))
					{
						m_FileBrowser = std::make_unique<ImGui::FileBrowser>(ImGuiFileBrowserFlags_EnterNewFilename);
						m_SelectionState = AnimationPanelSelectionState::Save;

						m_FileBrowser->SetTitle("Save as");
						m_FileBrowser->SetTypeFilters({ ".smolanim" });
						m_FileBrowser->Open();
					}

					if (ImGui::MenuItem("Load"))
					{
						m_FileBrowser = std::make_unique<ImGui::FileBrowser>();
						m_SelectionState = AnimationPanelSelectionState::Load;

						m_FileBrowser->SetTitle("Load Clip");
						m_FileBrowser->SetTypeFilters({ ".smolanim" });
						m_FileBrowser->Open();
					}

					if (ImGui::BeginMenu("Tools"))
					{
						if (ImGui::MenuItem("Set Global Parameters"))
						{
							showTools = true;
						}

						ImGui::EndMenu();
					}

					if (ImGui::MenuItem("Help"))
					{

					}

				}
				ImGui::EndMenuBar();
				ImGui::PopStyleVar();

				ImGui::NewLine();
				ImGui::InputTextWithHint("Clip Name", m_AnimationClip->m_ClipName.c_str(), &m_AnimationClip->m_ClipName);

				ImGui::NewLine();

				if (ImGui::BeginChild("FrameSettings"))
				{

					if (ImGui::TreeNodeEx("Frames", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Bullet))
					{

						for (auto pair : m_AnimationClip->m_Frames)
						{
							auto& [index, frame] = pair;

							std::string name = "Frame #" + std::to_string(index + 1);
							if (ImGui::TreeNodeEx(name.c_str()))
							{
								ImGui::InputFloat("Duration", &frame->Speed);

								if (ImGui::ButtonEx("Sprite"))
								{
									m_FileBrowser = std::make_unique<ImGui::FileBrowser>();

									m_SelectedFrame = frame;
									m_SelectionState = AnimationPanelSelectionState::NewSprite;

									m_FileBrowser->SetTitle("Select a sprite");
									m_FileBrowser->SetTypeFilters({ ".png" });
									m_FileBrowser->Open();
								}

								ImGui::SameLine();
								ImGui::BulletText(frame->FileName.c_str());

								ImGui::ColorEdit3("Color", glm::value_ptr(frame->TextureColor));
								ImGui::InputFloat2("Scale", glm::value_ptr(frame->TextureScale));


								ImGui::TreePop();
							}

							if (ImGui::IsMouseDoubleClicked(1))
							{
								m_AnimationClip->m_Frames.erase(index);

								ImGui::TreePop();
								ImGui::EndChild();
								ImGui::End();
								return;
							}
						}

						ImGui::TreePop();
					}

					ImGui::EndChild();
				}

				if (showPreview)
				{
					ImGui::Begin("Preview", &showPreview);
					{
						ImGui::SetWindowSize("Preview", {280, 350 });

						static float zoomLevel = 1.0f;
						ImGui::InputFloat("Zoom Level", &zoomLevel);
						if (m_Camera->m_ZoomLevel != zoomLevel)
						{
							m_Camera->SetZoom(zoomLevel);
						}

						ImGui::BeginChild("Anim");
						{
							if (m_AnimationClip->m_IsActive)
							{
								Animation2DSystem::DebugUpdate(m_AnimationClip.get());
							}

							m_Camera->GetFramebuffer()->Bind();
							RendererCommand::SetClearColor({ 0.0f, 0.0f, 0.0, 1 });
							RendererCommand::Clear();

							if (m_AnimationClip->m_CurrentTexture != nullptr && m_AnimationClip->m_CurrentFrameKey)
							{
								auto frameKey = m_AnimationClip->m_CurrentFrameKey;
								Renderer2D::DrawAnimation2DPreview(m_Camera->GetCamera(), 1.0f, { 0, 0, 1 }, frameKey->TextureScale, 0, m_AnimationClip->m_CurrentTexture, 1.0f, frameKey->TextureColor);
							}

							m_Camera->GetFramebuffer()->UnBind();
							size_t textureID = m_Camera->GetFramebuffer()->GetColorAttachmentID();
							ImGui::Image((void*)textureID, ImVec2{ 280, 280 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
						}
						ImGui::EndChild();
					}

					ImGui::End();
				}
			}


			ImGui::End();

			if(showTools)
			{
				ImGui::Begin("Tools", &showTools);
				{
					ImGui::SetWindowSize("Tools", { 240, 140 });

					static float time = 0;
					ImGui::InputFloat("Duration", &time);

					if (ImGui::Button("OK"))
					{
						for (auto pair: m_AnimationClip->m_Frames)
						{
							auto& [key, frame] = pair;
							frame->Speed = time;
						}

						showTools = false;
					}
				}
				ImGui::End();
			}

			m_FileBrowser->Display();

			if (m_FileBrowser->HasSelected())
			{

				auto FilePath = m_FileBrowser->GetSelected().u8string();
				auto FileName = m_FileBrowser->GetSelected().filename().u8string();

				switch (m_SelectionState)
				{
				case AnimationPanelSelectionState::NewFrame:
				{
					for (auto item : m_FileBrowser->GetMultiSelected())
					{
						FilePath = item.u8string();
						FileName = item.filename().u8string();

						m_SelectedFrame = std::make_shared<Animation2DFrameKey>();
						m_AnimationClip->m_Frames[m_AnimationClip->m_Frames.size()] = m_SelectedFrame;

						m_SelectedFrame->Texture = Texture2D::Create(FilePath);
						m_SelectedFrame->TexturePath = FilePath;
						m_SelectedFrame->FileName = FileName;

						m_SelectedFrame = nullptr;
						m_FileBrowser->ClearSelected();
					}

					m_SelectionState = AnimationPanelSelectionState::None;
					break;
				}
				case AnimationPanelSelectionState::NewSprite:
				{
					m_SelectedFrame->Texture = Texture2D::Create(FilePath);
					m_SelectedFrame->TexturePath = FilePath;
					m_SelectedFrame->FileName = FileName;

					m_SelectedFrame = nullptr;
					m_FileBrowser->ClearSelected();

					m_SelectionState = AnimationPanelSelectionState::None;
					break;
				}

				case AnimationPanelSelectionState::Save:
				{
					Save(FilePath);
					m_SelectionState = AnimationPanelSelectionState::None;
					break;
				}
				case AnimationPanelSelectionState::Load:
				{
					Load(FilePath);
					m_SelectionState = AnimationPanelSelectionState::None;
					break;
				}
				default:
					break;
				}
			}
		}
		else
		{
			if (m_AnimationClip != nullptr)
			{
				m_AnimationClip = nullptr;
			}
		}
	}

	void AnimationPanel::Save(const std::string& filePath)
	{
		std::stringstream storage;

		{
			cereal::JSONOutputArchive output{ storage };
			m_AnimationClip->serialize(output);
		}

		std::ofstream myfile(filePath);

		if (myfile.is_open())
		{
			myfile << storage.str();
			myfile.close();
			CONSOLE_WARN(std::string("Clip saved successfully"));
		}
		else
		{
			CONSOLE_ERROR(std::string("Clip Editor: Could not write to a file!"));
		}
	}

	void AnimationPanel::Load(const std::string& filePath)
	{
		std::ifstream file(filePath);
		std::stringstream storage;

		if (file)
		{
			storage << file.rdbuf();
			file.close();
		}
		else
		{
			CONSOLE_ERROR(std::string("Clip Editor: Could not open the file!"));
			return;
		}

		m_AnimationClip = std::make_unique<AnimationClip>();

		{
			cereal::JSONInputArchive sceneDataInput{ storage };
			sceneDataInput(m_AnimationClip->m_Frames, m_AnimationClip->m_ClipName);
		}

		for (auto pair: m_AnimationClip->m_Frames)
		{
			auto& [index, frame] = pair;

			if (AssetManager::PathCheck(frame->TexturePath, frame->FileName))
			{
				frame->Texture = Texture2D::Create(frame->TexturePath);
			}
		}
	}
}