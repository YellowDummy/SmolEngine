#include "stdafx.h"
#include "FileExplorer.h"

#include <Frostium3D/Common/Texture.h>
#include <imgui/imgui.h>

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
			DrawHierarchy();
		}
		ImGui::End();
	}

	void FileExplorer::DrawDirectory(const std::filesystem::path& path)
	{
		for (auto& file_path : std::filesystem::directory_iterator(path))
		{
			if (std::filesystem::is_directory(file_path))
			{
				std::string fileName = file_path.path().filename().u8string();
				// Folder icon
				ImGui::Image(m_TextureLoader->m_FolderButton.GetImGuiTexture(), { m_ButtonSize.x, m_ButtonSize.y }, ImVec2(1, 1), ImVec2(0, 0));
				ImGui::SameLine();

				bool open = ImGui::TreeNodeEx(fileName.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);
				if (open)
				{
					for (auto& child_file_path : std::filesystem::directory_iterator(file_path))
					{
						auto p = child_file_path.path();
						if (std::filesystem::is_directory(p))
						{
							DrawDirectory(p);
						}
						else
						{
							DrawNode(p);
						}
					}

					ImGui::TreePop();
				}
			}
		}
	}

	void FileExplorer::DrawNode(const std::filesystem::path& path)
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
				auto& it = m_IconsMap.find(texPath);
				if (it == m_IconsMap.end())
				{
					icon = std::make_shared<Texture>();
					Texture::Create(texPath, icon.get(), TextureFormat::R8G8B8A8_UNORM, true, true);
					m_IconsMap[texPath] = icon;
				}
				else
				{
					icon = it->second;
				}

				if (icon != nullptr)
				{
					ImGui::Image(icon->GetImGuiTexture(), { m_ButtonSize.x, m_ButtonSize.y }, ImVec2(1, 1), ImVec2(0, 0));
				}
			}
			else if (ext == ".s_scene")
			{
				ImGui::Image(m_TextureLoader->m_DocumentsButton.GetImGuiTexture(), { m_ButtonSize.x, m_ButtonSize.y }, ImVec2(1, 1), ImVec2(0, 0));
			}
			else if (ext == ".s_material")
			{
				ImGui::Image(m_TextureLoader->m_DocumentsButton.GetImGuiTexture(), { m_ButtonSize.x, m_ButtonSize.y }, ImVec2(1, 1), ImVec2(0, 0));
			}
			else if (ext == ".gltf")
			{
				ImGui::Image(m_TextureLoader->m_DocumentsButton.GetImGuiTexture(), { m_ButtonSize.x, m_ButtonSize.y }, ImVec2(1, 1), ImVec2(0, 0));
			}
			else
			{
				ImGui::Image(m_TextureLoader->m_DocumentsButton.GetImGuiTexture(), { m_ButtonSize.x, m_ButtonSize.y }, ImVec2(1, 1), ImVec2(0, 0));
			}

			ImGui::SameLine();
			bool open = ImGui::TreeNodeEx(name.c_str(),
				selected ? ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_Bullet : ImGuiTreeNodeFlags_Bullet);

			if (selected) { ImGui::PopStyleColor(); }
			if (ImGui::IsItemClicked())
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
				else
				{
					DrawNode(p);
				}
			}

			ImGui::TreePop();
		}
	}
}