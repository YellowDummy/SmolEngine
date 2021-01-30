#include "stdafx.h"
#include "BuildPanel.h"
#include "Core/SLog.h"

#include "ImGui/EditorConsole.h"
#include "Core/AssetManager.h"
#include "ECS/Components/Singletons/ProjectConfigSComponent.h"

#include <cereal/archives/json.hpp>
#include <cstdlib>

namespace SmolEngine
{
	BuildPanel::BuildPanel()
	{
		m_FileBrowser = std::make_unique<ImGui::FileBrowser>();
	}

	void BuildPanel::Update(bool& isOpen)
	{
		if (isOpen)
		{
			ImGui::Begin("Build Config", &isOpen, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize);
			{
				ImGui::SetWindowSize({ 720, 480 });

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 10.0f, 10.0f });
				if (ImGui::BeginMenuBar())
				{
					if (ImGui::MenuItem("Add Scene"))
					{
						m_FileBrowserState = BuildPanelFileBrowserState::Add_Scene;
						m_FileBrowser->SetTypeFilters({ ".smolscene" });
						m_FileBrowser->SetTitle("Load Scene");
						m_FileBrowser->Open();
					}

					if (ImGui::MenuItem("Save"))
					{
						m_FileBrowser = std::make_unique<ImGui::FileBrowser>(ImGuiFileBrowserFlags_EnterNewFilename);

						m_FileBrowserState = BuildPanelFileBrowserState::Save_Config;
						m_FileBrowser->SetTypeFilters({ ".smolconfig" });
						m_FileBrowser->SetTitle("Save as");
						m_FileBrowser->Open();
					}

					if (ImGui::MenuItem("Load"))
					{
						m_FileBrowserState = BuildPanelFileBrowserState::Load_Config;
						m_FileBrowser->SetTypeFilters({ ".smolconfig" });
						m_FileBrowser->SetTitle("Load Config");
						m_FileBrowser->Open();
					}

					if (ImGui::MenuItem("Clear"))
					{
						ProjectConfigSComponent* ref = ProjectConfigSComponent::Get();
						if (ref != nullptr)
						{
							ref->m_Scenes.clear();
						}
					}

				}
				ImGui::EndMenuBar();
				ImGui::PopStyleVar();

				ImGui::NewLine();

				ImGui::SetCursorPosX(115.0f);
				ImGui::Text("Scene");
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetWindowSize().x - m_xOffset + 110.0f);
				ImGui::Text("Index");
				ImGui::NewLine();

				ImGui::BeginChild("Scene Index", { ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - 200.0f });
				{
					ProjectConfigSComponent* ref = ProjectConfigSComponent::Get();

					if (ref != nullptr)
					{
						for (const auto& pair : ref->m_Scenes)
						{
							const auto& [key, scene] = pair;
							if (key == 0)
							{
								ImGui::NewLine();
							}

							ImGui::SetCursorPosX(50.0f);
							ImGui::Text(scene.FileName.c_str());
							ImGui::SameLine();
							ImGui::SetCursorPosX(ImGui::GetWindowSize().x - m_xOffset + 120.0f);
							ImGui::Text(std::to_string(key).c_str());
							ImGui::NewLine();
						}
					}
				}
				ImGui::EndChild();

				ImGui::NewLine();

				ImGui::SetCursorPosY(ImGui::GetWindowSize().y - 50.0f);
				if (ImGui::Button("Build", { ImGui::GetWindowSize().x, 40.0f }))
				{
					//Build Solution
					if (Save("../Config/ProjectConfig.smolconfig"))
					{
						//TEMP
						system("cd C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/MSBuild/Current/Bin && msbuild.exe C:/Dev/SmolEngine/SmolEngine-Editor/GameBuild.sln /p:Configuration=Debug");
						system("C:/Dev/SmolEngine/Config/Build/Debug-windows-x86_64/GameX/GameX.exe");
					}
				}
				
			}
			ImGui::End();

			m_FileBrowser->Display();

			if (m_FileBrowser->HasSelected())
			{
				const auto filePath = m_FileBrowser->GetSelected().u8string();
				const auto fileName = m_FileBrowser->GetSelected().filename().u8string();

				switch (m_FileBrowserState)
				{
				case BuildPanelFileBrowserState::Add_Scene:
				{
					ProjectConfigSComponent* ref = ProjectConfigSComponent::Get();
					if (ref != nullptr)
					{
						SceneConfigData temp;
						temp.FileName = fileName;
						temp.FilePath = filePath;

						ref->m_Scenes[static_cast<uint32_t>(ref->m_Scenes.size())] = temp;
						ResetFileBrowser();
					}

					break;
				}
				case BuildPanelFileBrowserState::Save_Config:
				{
					Save(filePath);
					ResetFileBrowser();
					break;
				}
				case BuildPanelFileBrowserState::Load_Config:
				{
					Load(filePath);
					ResetFileBrowser();
					break;
				}
				default:
					break;
				}
			}
		}
	}

	void BuildPanel::Clear()
	{

	}

	void BuildPanel::ResetFileBrowser()
	{
		m_FileBrowser->ClearSelected();
		m_FileBrowserState = BuildPanelFileBrowserState::None;
	}

	bool BuildPanel::Save(const std::string& filePath)
	{
		ProjectConfigSComponent* ref = ProjectConfigSComponent::Get();
		if (ref == nullptr)
		{
			return false;
		}

		std::stringstream storage;

		{
			cereal::JSONOutputArchive output{ storage };
			ref->serialize(output);
		}

		std::ofstream myfile(filePath);
		if (myfile.is_open())
		{
			myfile << storage.str();
			myfile.close();
			CONSOLE_WARN(std::string("Project config saved successfully"));
			return true;
		}

		CONSOLE_ERROR(std::string("Could not write to a file!"));
		return false;
	}

	bool BuildPanel::Load(const std::string& filePath)
	{
		ProjectConfigSComponent* ref = ProjectConfigSComponent::Get();
		if (ref == nullptr)
		{
			return false;
		}

		std::stringstream storage;
		std::ifstream file(filePath);

		if (!file)
		{
			NATIVE_ERROR("Could not open the file: {}", filePath);
			return false;
		}

		storage << file.rdbuf();
		file.close();

		{
			cereal::JSONInputArchive dataInput{ storage };
			dataInput(ref->m_Scenes, ref->m_AssetFolder);
		}

		for (auto& pair: ref->m_Scenes)
		{
			auto& [key, data] = pair;
			if (!AssetManager::PathCheck(data.FilePath, data.FileName))
			{
				NATIVE_ERROR("Could not open the file: {}", data.FilePath);
				return false;
			}
		}

		return true;
	}
}