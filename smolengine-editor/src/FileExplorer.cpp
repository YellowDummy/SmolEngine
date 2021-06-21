#include "stdafx.h"
#include "FileExplorer.h"

#include <Frostium3D/Common/Texture.h>
#include <imgui/imgui.h>
#include <ECS/Systems/JobsSystem.h>

namespace SmolEngine
{
	void FileExplorer::Create(const std::string& current_path, TexturesLoader* loader)
	{
		m_TextureLoader = loader;
		m_CurrentDir = current_path;
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

	void FileExplorer::DrawDirectory(const std::filesystem::path& orig_path)
	{
		// Folder icon
		DrawIcon(&m_TextureLoader->m_FolderButton);

		std::string fileName = orig_path.filename().u8string();
		auto& it = m_OpenDirectories.find(fileName);
		bool found = it != m_OpenDirectories.end();
		bool open = ImGui::TreeNodeEx(fileName.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
		{
			m_DirPopUpPath = orig_path.u8string();
		}

		if (open)
		{
			if (found == false) { m_OpenDirectories[fileName] = Directory(); }
			for (auto& child_file_path : std::filesystem::directory_iterator(orig_path))
			{
				auto p = child_file_path.path();
				if (std::filesystem::is_directory(p))
				{
					DrawDirectory(p);
				}
				else
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

	void FileExplorer::DrawNode(const std::filesystem::path& path, Directory& owner)
	{
		std::string ext = path.extension().u8string();
		bool extSupported = std::find(m_FileExtensions.begin(), m_FileExtensions.end(), ext) != m_FileExtensions.end();
		if (extSupported)
		{
			std::string name = path.filename().u8string();
			bool selected = m_SelectedNode == name;
			if (selected) { ImGui::PushStyleColor(ImGuiCol_Text, { 0.1f, 0.3f, 1.0f, 1.0f }); }

			if (ext == ".png" || ext == ".jpg")
			{
				Ref<Texture> icon = nullptr;
				std::string texPath = path.u8string();
				auto& it = owner.m_IconsMap.find(texPath);
				if (it == owner.m_IconsMap.end())
				{
					icon = std::make_shared<Texture>();
					owner.m_IconsMap[texPath] = icon;

					JobsSystem::Schedule([&owner, texPath]()
					{
						auto tex = owner.m_IconsMap[texPath];
						Texture::Create(texPath, tex.get(), TextureFormat::R8G8B8A8_UNORM, true, true);
					});
				}
				else { icon = it->second; }
				if (icon != nullptr)
				{
					if (icon->GetWidth() > 0)
						DrawIcon(icon.get());
				}

			}
			else if (ext == ".s_scene")
			{
				DrawIcon(&m_TextureLoader->m_SceneIcon);
			}
			else if (ext == ".s_material")
			{
				DrawIcon(&m_TextureLoader->m_MaterialIcon);
			}
			else if (ext == ".gltf")
			{
				DrawIcon(&m_TextureLoader->m_glTFIcon);
			}
			else
			{
				DrawIcon(&m_TextureLoader->m_DocumentsIcon);
			}

			bool open = ImGui::TreeNodeEx(name.c_str(),
				selected ? ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_Bullet : ImGuiTreeNodeFlags_Bullet);

			if (selected) { ImGui::PopStyleColor(); }
			if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
			{
				m_NodePopUpPath = path.u8string();
			}
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			{
				m_SelectedNode = name;
				if (ImGui::IsMouseDoubleClicked(0))
				{

				}
			}

			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("FileBrowser", &m_SelectedNode, sizeof(std::string));
				ImGui::Text(m_SelectedNode.c_str());
				ImGui::EndDragDropSource();
			}

			if (open) { ImGui::TreePop(); }
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

	void FileExplorer::DrawPopUp()
	{
		if(m_DirPopUpPath.empty() == false)
			ImGui::OpenPopup("FileExplorerActions");

		if(m_NodePopUpPath.empty() == false)
			ImGui::OpenPopup("FileExplorerNodeActions");

		if (ImGui::BeginPopup("FileExplorerActions"))
		{
			m_DirPopUpPath = "";
			ImGui::MenuItem("Actions", NULL, false, false);
			ImGui::Separator();
			if (ImGui::MenuItem("New Material"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopup("FileExplorerNodeActions"))
		{
			m_NodePopUpPath = "";
			if (ImGui::MenuItem("delete"))
			{
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("rename"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
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