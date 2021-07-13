#include "stdafx.h"
#include "FileExplorer.h"
#include "MaterialInspector.h"

#include <ECS/Systems/JobsSystem.h>
#include <Core/CommandUtils.h>

#include <Frostium3D/Primitives/Texture.h>
#include <Frostium3D/MaterialLibrary.h>

#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace SmolEngine
{
	FileExplorer::FileExplorer()
		:
		m_ButtonSize(35.0f, 25.0f),
		m_SelectColor(0.984f, 0.952f, 0.356f, 1.0f)
	{

	}

	void FileExplorer::Create(const std::string& current_path)
	{
		m_pTextureLoader = TexturesLoader::Get();
		m_CurrentDir = current_path;
		m_HomeDir = m_CurrentDir;
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

		ImGui::Image(m_pTextureLoader->m_FolderButton.GetImGuiTexture(), { m_ButtonSize.x, m_ButtonSize.y }, ImVec2(1, 1), ImVec2(0, 0));
		ImGui::SameLine();

		DrawSelectable(fileName, filePath);

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			m_SelectedNode = filePath;

			if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				m_CurrentDir = filePath;
				Reset();
			}
		}
	}

	void FileExplorer::DrawNode(const std::filesystem::path& fs_path)
	{
		std::string name = fs_path.filename().u8string();
		std::string path = fs_path.u8string();
		std::string ext = fs_path.extension().u8string();

		bool extSupported = std::find(m_FileExtensions.begin(), m_FileExtensions.end(), ext) != m_FileExtensions.end();
		if (extSupported)
		{
			DrawIcon(path, ext);

			const bool is_action_pending = IsAnyActionPending(fs_path);
			if (is_action_pending == false)
			{
				DrawSelectable(name, path);
				 
				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					m_PopUpBuffer = path;
					m_ePopUpFlags = PopUpFlags::Node;
				}

				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						m_SelectedNode = path;

						if (m_pOnFileSelected != nullptr)
						{
							size_t fSize = GetNodeSize(path);
							m_pOnFileSelected(path, ext, static_cast<int>(fSize));
						}
					}
				}

				if (ImGui::BeginDragDropSource())
				{
					m_DragAndDropBuffer = path;
					ImGui::SetDragDropPayload("FileBrowser", &m_DragAndDropBuffer, sizeof(std::string));
					DrawIcon(path, ext);
					ImGui::Text(name.c_str());
					ImGui::EndDragDropSource();
				}
			}
			else
			{
				if (m_pPendeingAction->Type == PendeingActionFlags::NewFile || m_pPendeingAction->Type == PendeingActionFlags::Rename)
				{
					ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0.180f, 0.521f, 1.0f,  1.0f });
					if (ImGui::InputTextWithHint("##ActionsRenameNode", name.c_str(), &m_pPendeingAction->NewName, ImGuiInputTextFlags_EnterReturnsTrue))
					{
						if (m_pPendeingAction->NewName.empty() == false)
						{
							std::filesystem::rename(path, fs_path.parent_path().u8string() + "/" + m_pPendeingAction->NewName + ext);
						}

						switch (m_pPendeingAction->Type)
						{
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

	void FileExplorer::DrawIcon(const std::string& path, const std::string& ext)
	{
		void* descriptorPtr = nullptr;
		GetIcon(path, ext, descriptorPtr);

		if (descriptorPtr != nullptr)
		{
			ImGui::Image(descriptorPtr, { m_ButtonSize.x, m_ButtonSize.y });
			ImGui::SameLine();
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
				std::string newPath = m_CurrentDir + "/new_material" + ".s_material";
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

	bool FileExplorer::IsAnyActionPending(const std::filesystem::path& node_path)
	{
		if (m_pPendeingAction != nullptr)
		{
			std::filesystem::path p1(m_pPendeingAction->Path);
			std::string f1 = p1.filename().u8string();
			std::string f2 = node_path.filename().u8string();

			return f1 == f2;
		}

		return false;
	}

	size_t FileExplorer::GetNodeSize(const std::string& path)
	{
		std::ifstream in(path, std::ifstream::ate | std::ifstream::binary);
		return in.tellg();
	}

	void FileExplorer::GetIcon(const std::string& path, const std::string& ext, void*& descriptorPtr)
	{
		if (ext == ".s_image")
		{
			TextureCreateInfo texCI = {};
			texCI.Load(path);
			texCI.bImGUIHandle = true;
			bool is_ktx = texCI.FilePath.find("ktx") != std::string::npos;

			if (is_ktx == false)
			{
				auto& it = m_IconsMap.find(path);
				if (it == m_IconsMap.end())
				{
					auto icon = std::make_shared<Texture>();
					m_IconsMap[path] = icon;
					JobsSystem::Schedule([this, path, texCI]()
					{
						auto tex = m_IconsMap[path];
						Texture::Create(&texCI, tex.get());
					});
				}
				else 
				{ 
					descriptorPtr = it->second->GetImGuiTexture();
				}
			}
			else
			{
				descriptorPtr = m_pTextureLoader->m_DocumentsIcon.GetImGuiTexture();
			}
		}
		else if (ext == ".s_material")
		{
			auto& it = m_MaterialPreviews.find(path);
			if (it == m_MaterialPreviews.end()) // Generates material icon
			{
				size_t fileSze = GetNodeSize(path);
				if (fileSze > 0)
				{
					auto fb = std::make_shared<Framebuffer>();
					{
						FramebufferSpecification framebufferCI = {};
						FramebufferAttachment colorAttach = FramebufferAttachment(AttachmentFormat::Color, true);
						colorAttach.ClearColor = glm::vec4(0.1, 0.1, 0.1, 1);

						framebufferCI.Width = 256;
						framebufferCI.Height = 256;
						framebufferCI.bResizable = false;
						framebufferCI.bUsedByImGui = true;
						framebufferCI.bAutoSync = false;
						framebufferCI.Attachments = { colorAttach };
						framebufferCI.eMSAASampels = MSAASamples::SAMPLE_COUNT_1;

						Framebuffer::Create(framebufferCI, fb.get());
					}

					MaterialCreateInfo materailCI = {};
					if (materailCI.Load(path) == true)
					{
						MaterialInspector::GetSingleton()->RenderMaterialIcon(fb.get(), &materailCI);
						m_MaterialPreviews[path] = fb;
					}
				}
			}
			else
			{
				descriptorPtr = it->second->GetImGuiTextureID();
			}
		}
		else if (ext == ".s_scene")
		{
			descriptorPtr = m_pTextureLoader->m_SceneIcon.GetImGuiTexture();
		}
		else if (ext == ".s_material")
		{
			descriptorPtr = m_pTextureLoader->m_MaterialIcon.GetImGuiTexture();
		}
		else if (ext == ".gltf")
		{
			descriptorPtr = m_pTextureLoader->m_glTFIcon.GetImGuiTexture();
		}
		else
		{
			descriptorPtr = m_pTextureLoader->m_DocumentsIcon.GetImGuiTexture();
		}
	}

	void FileExplorer::ClosePopUp()
	{
		ImGui::CloseCurrentPopup();
		m_PopUpBuffer = "";
	}

	void FileExplorer::Reset()
	{
		m_MaterialPreviews.clear();
		m_IconsMap.clear();
		m_SelectedNode.clear();
		m_PopUpBuffer.clear();

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
			{
				bool active = std::filesystem::equivalent(m_CurrentDir, m_HomeDir) == false;
				if (active == false) { ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f); }

				if (ImGui::ImageButton(m_pTextureLoader->m_ReturnIcon.GetImGuiTexture(), { 25, 25 }, ImVec2(0, 1), ImVec2(1, 0)))
				{
					if (active)
					{
						std::filesystem::path p(m_CurrentDir);
						m_CurrentDir = p.parent_path().u8string();
						Reset();
					}
				}

				if (active == false) { ImGui::PopStyleVar(); }
			}

			ImGui::SameLine();
			if (ImGui::ImageButton(m_pTextureLoader->m_ForwardIcon.GetImGuiTexture(), { 25, 25 }, ImVec2(0, 1), ImVec2(1, 0)))
			{
				if (m_SelectedNode.empty() == false)
				{
					m_CurrentDir = m_SelectedNode;
					Reset();
				}
			}

			ImGui::SameLine();
			if (ImGui::ImageButton(m_pTextureLoader->m_UpdateIcon.GetImGuiTexture(), { 25, 25 }, ImVec2(0, 1), ImVec2(1, 0)))
			{
				Import();
			}

			ImGui::SameLine();
			ImGui::TextDisabled("(?)");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(450.0f);
				ImGui::TextUnformatted("import");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}

			ImGui::SameLine();

			float pos = ImGui::GetCursorPosY();
			ImGui::SetCursorPosY(pos + 3);
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 200);
			ImGui::InputTextWithHint("Filter", "search", &m_SearchBuffer);
			ImGui::Separator();
		}

		if (m_SelectedNode.empty())
		{
			for (auto& file_path : std::filesystem::directory_iterator(m_CurrentDir))
			{
				if (std::filesystem::is_directory(file_path))
				{
					auto p = file_path.path();
					m_SelectedNode = p.u8string();
					break;
				}
			}
		}

		// first draw directories
		for (auto& file_path : std::filesystem::directory_iterator(m_CurrentDir))
		{
			if (std::filesystem::is_directory(file_path))
			{
				auto p = file_path.path();
				DrawDirectory(p);
			}
		}

		for (auto& file_path : std::filesystem::directory_iterator(m_CurrentDir))
		{
			if (std::filesystem::is_directory(file_path) == false)
			{
				auto p = file_path.path();
				DrawNode(p);
			}
		}

		DrawPopUp();
	}

	void FileExplorer::DrawSelectable(const std::string& name, const std::string& path)
	{
		bool selected = m_SelectedNode == path;
		if (selected) { ImGui::PushStyleColor(ImGuiCol_Text, m_SelectColor); }
		{
			ImGui::Selectable(name.c_str(), selected);
		}
		if (selected) { ImGui::PopStyleColor(); }
	}
}