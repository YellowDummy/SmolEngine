#include "stdafx.h"
#include "FileExplorer.h"

#include <ECS/Systems/JobsSystem.h>
#include <Core/CommandUtils.h>
#include <Frostium3D/Primitives/Texture.h>

#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace SmolEngine
{
	void FileExplorer::Create(const std::string& current_path)
	{
		m_pTextureLoader = TexturesLoader::Get();
		m_CurrentDir = current_path;
		Import();
	}

	void FileExplorer::ClearSelection()
	{
		m_SelectedNode = "";
		ClosePopUp();
	}

	void FileExplorer::Update()
	{
		ImGui::Begin("File Explorer", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
		{
			JobsSystem::BeginSubmition();
			{
				DrawHierarchy();
			}
			JobsSystem::EndSubmition();
		}
		ImGui::End();
	}

	void FileExplorer::Import()
	{
		for (auto& p : std::filesystem::recursive_directory_iterator(m_CurrentDir))
		{
			if (std::filesystem::is_directory(p) == false)
			{
				std::filesystem::path path = p.path();
				std::string ext = path.extension().u8string();

				if (ext == ".jpg" || ext == ".png" || ext == ".ktx")
				{
					std::string new_path = path.parent_path().u8string() + "/" + path.stem().u8string() + ".s_image";
					if (std::filesystem::exists(new_path) == false)
					{
						TextureCreateInfo texInfo = {};
						texInfo.FilePath = path.u8string();
						texInfo.Save(new_path);
					}
				}

				if (ext == ".gltf")
				{

				}
			}
		}
	}

	void FileExplorer::SetOnFileSelectedCallback(const std::function<void(const std::string&, const std::string&, int)>& callback)
	{
		m_pOnFileSelected = callback;
	}

	void FileExplorer::SetOnFileDeletedCallaback(const std::function<void(const std::string&, const std::string&)>& callback)
	{
		m_pOnFileDeleted = callback;
	}

	void FileExplorer::DrawDirectory(const std::filesystem::path& orig_path)
	{
		std::string fileName = orig_path.filename().u8string();
		std::string filePath = orig_path.u8string();
		if (m_SelectedDir.empty())
		{
			m_SelectedDir = filePath;
		}

		// Folder icon
		DrawIcon(&m_pTextureLoader->m_FolderButton);
		auto& it = m_OpenDirectories.find(fileName);
		bool found = it != m_OpenDirectories.end();
		bool open = ImGui::TreeNodeEx(fileName.c_str(), m_SelectedDir == filePath ? 
			ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_OpenOnArrow : ImGuiTreeNodeFlags_OpenOnArrow);

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			m_SelectedDir = filePath;
		}

		if (open)
		{
			if (found == false) { m_OpenDirectories[fileName] = Directory(); }

			// first draw directories
			for (auto& child_file_path : std::filesystem::directory_iterator(orig_path))
			{
				auto p = child_file_path.path();
				if (std::filesystem::is_directory(p) == true)
				{
					DrawDirectory(p);
				}
			}

			// files
			for (auto& child_file_path : std::filesystem::directory_iterator(orig_path))
			{
				auto p = child_file_path.path();
				std::string child_fm = p.filename().stem().u8string();
				if ((child_fm.find(m_SearchBuffer) != std::string::npos))
				{
					if (std::filesystem::is_directory(p) == false)
					{
						DrawNode(p, m_OpenDirectories[fileName]);
					}
				}
			}

			ImGui::TreePop();
		}
		else
		{
			if (found)
				m_OpenDirectories.erase(fileName);
		}
	}

	void FileExplorer::DrawNode(const std::filesystem::path& fs_path, Directory& owner)
	{
		Ref<Texture> icon = nullptr;
		std::string name = fs_path.filename().stem().u8string();
		std::string path = fs_path.u8string();
		std::string ext = fs_path.extension().u8string();

		bool extSupported = std::find(m_FileExtensions.begin(), m_FileExtensions.end(), ext) != m_FileExtensions.end();
		if (extSupported)
		{
			DrawNodeIcon(path, ext, owner, icon);

			bool is_action_pending = false;
			if (m_pPendeingAction != nullptr)
			{
				std::filesystem::path p1(m_pPendeingAction->Path);
				std::string f1 = p1.filename().u8string();
				std::string f2 = fs_path.filename().u8string();

				is_action_pending = f1 == f2;
			}

			if (is_action_pending == false)
			{
				bool selected = m_SelectedNode == name;
				if (selected) { ImGui::PushStyleColor(ImGuiCol_Text, { 0.984f, 0.952f, 0.356f, 1.0f }); }
				bool open = ImGui::TreeNodeEx(name.c_str(),
					selected ? ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_Bullet : ImGuiTreeNodeFlags_Bullet);
				if (selected) { ImGui::PopStyleColor(); }
				 
				// Pop Up
				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					m_PopUpBuffer = path;
					m_ePopUpFlags = PopUpFlags::Node;
				}

				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						m_SelectedNode = name;
						if (m_pOnFileSelected != nullptr)
						{
							std::ifstream in(path, std::ifstream::ate | std::ifstream::binary);
							size_t fSize = in.tellg();
							m_pOnFileSelected(path, ext, static_cast<int>(fSize));
						}
					}
				}

				if (ImGui::BeginDragDropSource())
				{
					m_DragAndDropBuffer = path;
					ImGui::SetDragDropPayload("FileBrowser", &m_DragAndDropBuffer, sizeof(std::string));
					if (icon != nullptr)
					{
						if (icon->GetInfo().Width > 0)
							DrawIcon(icon.get());
					}
					else
					{
						DrawIcon(ext);
					}

					ImGui::Text(name.c_str());
					ImGui::EndDragDropSource();
				}

				if (open) { ImGui::TreePop(); }
			}
			else
			{
				if (m_pPendeingAction->Type == PendeingActionFlags::NewFile || m_pPendeingAction->Type == PendeingActionFlags::Rename)
				{
					float posX = ImGui::GetCursorPosX();
					ImGui::SetCursorPosX(posX + 10.0f);

					ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0.180f, 0.521f, 1.0f,  1.0f });
					if (ImGui::InputTextWithHint("##ActionsRenameNode", name.c_str(), &m_pPendeingAction->NewName, ImGuiInputTextFlags_EnterReturnsTrue))
					{
						if (m_pPendeingAction->NewName.empty() == false)
						{
							std::filesystem::rename(path, fs_path.parent_path().u8string() + "/" + m_pPendeingAction->NewName + ext);
						}

						switch (m_pPendeingAction->Type)
						{
						case PendeingActionFlags::Rename:
						{

						}
						case PendeingActionFlags::NewFile:
						{
							if (m_pPendeingAction->NewName.empty() == true)
							{
								std::filesystem::remove(path);
							}

							break;
						}
						}

						delete m_pPendeingAction;
						m_pPendeingAction = nullptr;
					}
					ImGui::PopStyleColor();
				}
			}
		}
	}

	void FileExplorer::DrawIcon(Texture* icon, bool flip)
	{
		if(flip)
			ImGui::Image(icon->GetImGuiTexture(), { m_ButtonSize.x, m_ButtonSize.y });
		else
			ImGui::Image(icon->GetImGuiTexture(), { m_ButtonSize.x, m_ButtonSize.y }, ImVec2(1, 1), ImVec2(0, 0));

		ImGui::SameLine();
	}

	void FileExplorer::DrawIcon(const std::string& ext)
	{
		if (ext == ".s_scene")
		{
		   DrawIcon(&m_pTextureLoader->m_SceneIcon);
		}
		else if (ext == ".s_material")
		{
		   DrawIcon(&m_pTextureLoader->m_MaterialIcon);
		}
		else if (ext == ".gltf")
		{
		   DrawIcon(&m_pTextureLoader->m_glTFIcon);
		}
		else
		{
			DrawIcon(&m_pTextureLoader->m_DocumentsIcon);
		}
	}

	void FileExplorer::DrawNodeIcon(const std::string& path, const std::string& ext, Directory& owner, Ref<Texture>& icon)
	{
		bool defaultIcon = true;
		if (ext == ".s_image")
		{
			TextureCreateInfo texCI = {};
			texCI.Load(path);
			texCI.bImGUIHandle = true;
			bool is_ktx = texCI.FilePath.find("ktx") != std::string::npos;

			if (is_ktx == false)
			{
				auto& it = owner.m_IconsMap.find(path);
				if (it == owner.m_IconsMap.end())
				{
					icon = std::make_shared<Texture>();
					owner.m_IconsMap[path] = icon;

					JobsSystem::Schedule([&owner, path, texCI]()
					{
						auto tex = owner.m_IconsMap[path];
						Texture::Create(&texCI, tex.get());
					});
				}
				else { icon = it->second; }

				if (icon != nullptr)
				{
					if (icon->IsReady())
					{
						DrawIcon(icon.get());
						defaultIcon = false;
					}
				}
			}
		}

		if (defaultIcon == true)
		{
			DrawIcon(ext);
		}
	}

	void FileExplorer::DrawPopUp()
	{
		switch (m_ePopUpFlags)
		{
		case PopUpFlags::None:  if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) { ImGui::OpenPopup("FileExplorerActions"); }; break;
		case PopUpFlags::Node: 	if (m_pPendeingAction == nullptr) { ImGui::OpenPopup("FileExplorerNodeActions"); }; break;
		}

		if (ImGui::BeginPopup("FileExplorerActions"))
		{
			ImGui::MenuItem("actions", NULL, false, false);
			ImGui::Separator();

			if (ImGui::MenuItem("new material"))
			{
				std::string newPath = m_SelectedDir + "/new_material" + ".s_material";
				AddPendingAction(newPath, PendeingActionFlags::NewFile);
				ClosePopUp();
			}

			if (ImGui::MenuItem("new shader"))
			{
				ClosePopUp();
			}

			if (ImGui::MenuItem("new script"))
			{
				ClosePopUp();
			}

			m_ePopUpFlags = PopUpFlags::None;
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopup("FileExplorerNodeActions"))
		{
			ImGui::MenuItem("actions", NULL, false, false);
			ImGui::Separator();
			if (ImGui::MenuItem("rename"))
			{
				AddPendingAction(m_PopUpBuffer, PendeingActionFlags::Rename);
				ClosePopUp();
			}

			if (ImGui::BeginMenu("delete"))
			{
				std::filesystem::path p(m_PopUpBuffer);
				ImGui::NewLine();
				ImGui::TextWrapped("delete %s?", p.filename().stem().u8string().c_str());
				ImGui::SameLine();
				float posY = ImGui::GetCursorPosY();
				ImGui::SetCursorPosY(posY - 10);
				if (ImGui::Button("confirm", { 70, 35 }))
				{
					if (m_pOnFileDeleted != nullptr)
						m_pOnFileDeleted(m_PopUpBuffer, p.extension().u8string());

					std::filesystem::remove(m_PopUpBuffer);
					ClosePopUp();
				}

				ImGui::EndMenu();
			}

			m_ePopUpFlags = PopUpFlags::None;
			ImGui::EndPopup();
		}
	}

	void FileExplorer::AddPendingAction(const std::string& path, PendeingActionFlags action)
	{
		m_pPendeingAction = new PendeingAction();
		m_pPendeingAction->Type = action;
		m_pPendeingAction->Path = path;

		switch (action)
		{
		case PendeingActionFlags::NewFile:
		{
			if (std::filesystem::exists(path) == false)
			{
				std::ofstream outfile(m_pPendeingAction->Path);
				outfile.close();
				return;
			}

			break;
		}
		case PendeingActionFlags::Rename: return;
		}

		delete m_pPendeingAction;
		m_pPendeingAction = nullptr;
	}

	void FileExplorer::ClosePopUp()
	{
		ImGui::CloseCurrentPopup();
		m_PopUpBuffer = "";
	}

	void FileExplorer::Reset()
	{
		m_SelectedNode = "";
		m_PopUpBuffer = "";
		if (m_pPendeingAction != nullptr)
		{
			delete m_pPendeingAction;
			m_pPendeingAction = nullptr;
		}
		ImGui::CloseCurrentPopup();
	}

	void FileExplorer::DrawHierarchy()
	{
		{
			ImGui::Image(m_pTextureLoader->m_SearchButton.GetImGuiTexture(), { 25, 25 }, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::SameLine();

			float pos = ImGui::GetCursorPosY();
			ImGui::SetCursorPosY(pos + 3);
			ImGui::InputTextWithHint("Search", "", &m_SearchBuffer);
			ImGui::Separator();
		}

		std::string root = "Root (" + m_CurrentDir + ")";
		if (ImGui::TreeNodeEx(root.c_str(), ImGuiTreeNodeFlags_Leaf))
		{
			for (auto& file_path : std::filesystem::directory_iterator(m_CurrentDir))
			{
				auto p = file_path.path();
				if (std::filesystem::is_directory(file_path))
				{
					DrawDirectory(p);
				}
			}

			ImGui::TreePop();
		}

		DrawPopUp();
	}
}