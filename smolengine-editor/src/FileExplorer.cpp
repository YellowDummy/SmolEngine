#include "stdafx.h"
#include "FileExplorer.h"

#include <ECS/Systems/JobsSystem.h>
#include <Core/CommandUtils.h>
#include <Frostium3D/Common/Texture.h>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace SmolEngine
{
	void FileExplorer::Create(const std::string& current_path, TexturesLoader* loader)
	{
		m_pTextureLoader = loader;
		m_CurrentDir = current_path;
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
		// Folder icon
		DrawIcon(&m_pTextureLoader->m_FolderButton);

		std::string fileName = orig_path.filename().u8string();
		auto& it = m_OpenDirectories.find(fileName);
		bool found = it != m_OpenDirectories.end();
		bool open = ImGui::TreeNodeEx(fileName.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
		{
			m_PopUpBuffer = orig_path.u8string();
			m_ePopUpFlags = PopUpFlags::Folder;
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

			for (auto& child_file_path : std::filesystem::directory_iterator(orig_path))
			{
				auto p = child_file_path.path();
				if (std::filesystem::is_directory(p) == false)
				{
					DrawNode(p, m_OpenDirectories[fileName]);
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
		std::string name = fs_path.filename().u8string();
		std::string path = fs_path.u8string();
		std::string ext = fs_path.extension().u8string();

		bool extSupported = std::find(m_FileExtensions.begin(), m_FileExtensions.end(), ext) != m_FileExtensions.end();
		if (extSupported)
		{
			if (ext == ".png" || ext == ".jpg")
			{
				auto& it = owner.m_IconsMap.find(path);
				if (it == owner.m_IconsMap.end())
				{
					icon = std::make_shared<Texture>();
					owner.m_IconsMap[path] = icon;
					JobsSystem::Schedule([&owner, path]()
					{
						auto tex = owner.m_IconsMap[path];
						Texture::Create(path, tex.get(), TextureFormat::R8G8B8A8_UNORM, true, true);
					});
				}
				else { icon = it->second; }
				if (icon != nullptr)
				{
					if (icon->GetWidth() > 0)
						DrawIcon(icon.get());
				}

			}
			else 
			{
				DrawIcon(ext);
			}

			bool is_action_pending = false;
			if (m_pPendeingAction != nullptr)
				is_action_pending = m_pPendeingAction->Path == path;

			if (is_action_pending == false)
			{
				bool selected = m_SelectedNode == name;
				if (selected) { ImGui::PushStyleColor(ImGuiCol_Text, { 0.1f, 0.3f, 1.0f, 1.0f }); }

				bool open = ImGui::TreeNodeEx(name.c_str(),
					selected ? ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_Bullet : ImGuiTreeNodeFlags_Bullet);

				if (selected) { ImGui::PopStyleColor(); }
				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					m_PopUpBuffer = path;
					m_ePopUpFlags = PopUpFlags::Node;
				}
				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					if (ImGui::IsMouseDoubleClicked(0))
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
						if (icon->GetWidth() > 0)
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
				switch (m_pPendeingAction->Type)
				{
				case PendeingActionFlags::Rename:
				{
					float posX = ImGui::GetCursorPosX();
					ImGui::SetCursorPosX(posX + 25.0f);

					if (ImGui::InputTextWithHint("##ActionsRenameNode", "new name", &m_pPendeingAction->NewName, ImGuiInputTextFlags_EnterReturnsTrue))
					{
						if (m_pPendeingAction->NewName.empty() == false)
						{
							std::filesystem::rename(path, fs_path.parent_path().u8string() + "/" + m_pPendeingAction->NewName + ext);
						}

						delete m_pPendeingAction;
						m_pPendeingAction = nullptr;
					}
					break;
				}
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

	void FileExplorer::DrawPopUp()
	{
		switch (m_ePopUpFlags)
		{
		case PopUpFlags::Folder:  ImGui::OpenPopup("FileExplorerActions"); break;
		case PopUpFlags::Node: 	if (m_pPendeingAction == nullptr) { ImGui::OpenPopup("FileExplorerNodeActions"); }; break;
		}

		if (ImGui::BeginPopup("FileExplorerActions"))
		{
			ImGui::MenuItem("actions", NULL, false, false);
			ImGui::Separator();
			if (ImGui::MenuItem("new material"))
			{
				uint32_t num_dublicates = CommandUtils::GetNumFilenameDublicates("new_material", m_PopUpBuffer);
				std::string newPath = m_PopUpBuffer + "/new_material_" + std::to_string(num_dublicates) + ".s_material";
				if (std::filesystem::exists(newPath) == false)
				{
					std::ofstream outfile(newPath);
					outfile.close();
				}

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
				m_pPendeingAction = new PendeingAction();
				m_pPendeingAction->Type = PendeingActionFlags::Rename;
				m_pPendeingAction->Path = m_PopUpBuffer;

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
		if (ImGui::TreeNodeEx("Root", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_Leaf))
		{
			for (auto& file_path : std::filesystem::directory_iterator(m_CurrentDir))
			{
				auto p = file_path.path();
				if (std::filesystem::is_directory(file_path))
				{
					DrawDirectory(p);
				}
			}

			DrawPopUp();
			ImGui::TreePop();
		}
	}
}