#include "stdafx.h"
#include "Core/MaterialLibraryInterface.h"
#include "Core/FileDialog.h"

#include "ECS/Systems/JobsSystem.h"

#include <imgui/imgui.h>
#include <Frostium3D/ImGUI/ImGuiExtension.h>

namespace SmolEngine
{
	MaterialLibraryInterface::MaterialLibraryInterface()
	{
		InitPreviewRenderer();
	}

	void MaterialLibraryInterface::Draw(bool& show)
	{
		if (show)
		{
			ImGui::Begin("Material Library", &show, ImGuiWindowFlags_MenuBar);
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 10.0f, 10.0f });
				if (ImGui::BeginMenuBar())
				{
					if (ImGui::MenuItem("Save as"))
					{
						const auto& result = FileDialog::SaveFile("SmolEngine Material (*.s_material)\0*.s_material\0", "new_material.s_material");
						if (result.has_value())
						{

						}
					}

					if (ImGui::MenuItem("Load"))
					{
						Reset();
						const auto& result = FileDialog::OpenFile("SmolEngine Material (*.s_material)\0*.s_material\0");
						if (result.has_value())
						{

						}
					}

					if (ImGui::MenuItem("Show Preview"))
						m_bShowPreview = true;
				}
				ImGui::EndMenuBar();
				ImGui::PopStyleVar();

				ImGui::Extensions::Text("Textures", "");
				ImGui::NewLine();

				ImGui::Extensions::Text("Parameters", "");

				ImGui::NewLine();
				if (ImGui::Button("Generate Preview", { ImGui::GetWindowWidth() - 20.0f, 30.0f }))
				{
				}

				if (m_bShowPreview)
				{
					ImGui::Begin("Material Preview", &m_bShowPreview);
					ImGui::SetWindowSize({ 720, 540 });
					ImGui::End();
				}
			}
			ImGui::End();
		}
	}

	void MaterialLibraryInterface::Reset()
	{
		m_Buffer = {};
		m_MaterialCI = {};
	}

	void MaterialLibraryInterface::InitPreviewRenderer()
	{
	}

	void MaterialLibraryInterface::DrawTextureInfo(const char* header, std::string& outString, std::string& dummy)
	{
		std::string id = header + std::string("add");
		ImGui::PushID(id.c_str());
		if (ImGui::Extensions::InputRawString(header, dummy, header, 130.0f, true))
			outString = dummy;

		ImGui::SameLine();
		if(ImGui::SmallButton("Add"))
		{
			const auto& result = FileDialog::OpenFile("");
			if (result.has_value())
				dummy = result.value();
		}

		ImGui::SameLine();
		if (ImGui::SmallButton("Clear"))
		{
			dummy = "";
		}
		
		outString = dummy;
		ImGui::PopID();
	}
}