#include "stdafx.h"
#include "MaterialLibraryInterface.h"
#include "Core/FileDialog.h"

#include <imgui.h>
#include "ImGui/ImGuiExtension.h"
#include "ImGui/EditorConsole.h"

namespace SmolEngine
{
	void MaterialLibraryInterface::Draw(bool& show)
	{
		if (show)
		{
			ImGui::Begin("Material Library", &show);
			{
				if (ImGui::CollapsingHeader("Create New"))
				{
					if (ImGui::Extensions::InputRawString("Name", m_Buffer.name, "Material Name", 130.0f, false))
					{
						m_MaterialCI.Name = m_Buffer.name;
					}
					ImGui::NewLine();

					ImGui::Extensions::Text("Textures", "");

					DrawTextureInfo("Albedro", m_MaterialCI.Textures[MaterialTexture::Albedro], m_Buffer.albedro);
					DrawTextureInfo("Normal", m_MaterialCI.Textures[MaterialTexture::Normal], m_Buffer.normal);
					DrawTextureInfo("Metallic", m_MaterialCI.Textures[MaterialTexture::Metallic], m_Buffer.metallic);
					DrawTextureInfo("Roughness", m_MaterialCI.Textures[MaterialTexture::Roughness], m_Buffer.roughness);
					DrawTextureInfo("AO", m_MaterialCI.Textures[MaterialTexture::AO], m_Buffer.ao);
					ImGui::NewLine();

					ImGui::Extensions::Text("Parameters", "");
					ImGui::Extensions::InputFloat("Albedro", m_MaterialCI.Albedro);
					ImGui::Extensions::InputFloat("Metallic", m_MaterialCI.Metallic);
					ImGui::Extensions::InputFloat("Roughness", m_MaterialCI.Roughness);
					ImGui::Extensions::InputFloat("Specular", m_MaterialCI.Specular);

					ImGui::NewLine();
					if (ImGui::Button("Add", { ImGui::GetWindowWidth() - 20.0f, 30.0f }))
					{
						int32_t id = MaterialLibrary::GetSinglenton()->Add(&m_MaterialCI);
						if (id == -1)
						{
							std::string info = "Material " + m_MaterialCI.Name + " already exists!";
							CONSOLE_WARN(info);
						}
						else
						{
							std::string info = "Material " + m_MaterialCI.Name + " added successfully!";
							CONSOLE_INFO(info);
							MaterialLibrary::GetSinglenton()->Save();
						}

						m_Buffer = {};
						m_MaterialCI = {};
					}
				}

				ImGui::Separator();
				ImGui::Extensions::Text("Materials List", "");
				ImGui::NewLine();
				ImGui::BeginChild("Materials List");
				{
					for (uint32_t i = 0; i < static_cast<uint32_t>(MaterialLibrary::GetSinglenton()->m_SaveData.size()); i++)
					{
						auto& data = MaterialLibrary::GetSinglenton()->m_SaveData[i];
						auto& material = MaterialLibrary::GetSinglenton()->GetMaterials()[i];
						std::string name = "Material #" + data.Name;

						if (ImGui::CollapsingHeader(name.c_str()))
						{
							ImGui::Extensions::Text("Textures", "");
							ImGui::NewLine();

							ImGui::Extensions::Text("Albedro", data.Textures[MaterialTexture::Albedro]);
							ImGui::Extensions::Text("Normal", data.Textures[MaterialTexture::Normal]);
							ImGui::Extensions::Text("Metallic", data.Textures[MaterialTexture::Metallic]);
							ImGui::Extensions::Text("Roughness", data.Textures[MaterialTexture::Roughness]);
							ImGui::Extensions::Text("AO", data.Textures[MaterialTexture::AO]);
							ImGui::NewLine();

							ImGui::Extensions::Text("Parameters", "");
							ImGui::NewLine();
							ImGui::Extensions::InputFloat("Metallic", material.m_MaterialProperties.PBRValues.x);
							ImGui::Extensions::InputFloat("Roughness", material.m_MaterialProperties.PBRValues.y);
							ImGui::Extensions::InputFloat("Albedro", material.m_MaterialProperties.PBRValues.z);
							ImGui::Extensions::InputFloat("Specular", material.m_MaterialProperties.PBRValues.w);
						}
					}
				}
				ImGui::EndChild();
			}
			ImGui::End();
		}
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
			auto& result = FileDialog::OpenFile("");
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