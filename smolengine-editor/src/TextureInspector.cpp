#include "stdafx.h"
#include "TextureInspector.h"
#include "ImGuiExtension.h"

#include <imgui/imgui.h>

namespace SmolEngine
{
	void TextureInspector::Update()
	{
		if (m_Reload)
		{
			m_Info.Save(m_FilePath);

			delete m_Preview;
			m_Preview = nullptr;
			m_Reload = false;

			m_Preview = new Texture();
			TextureCreateInfo infoCopy = m_Info;
			infoCopy.bImGUIHandle = true;
			Texture::Create(&infoCopy, m_Preview);
		}

		ImGui::NewLine();

		ImGui::SetCursorPosX(12);
		ImGui::TextWrapped("Preview: %s", m_Titile.c_str());
		ImGui::Separator();
		ImGui::NewLine();
		ImGui::SetCursorPosX(12);
		ImGui::Image(m_Preview->GetImGuiTexture(), { ImGui::GetWindowWidth(), 280 });
		if (ImGui::IsItemHovered())
		{
			float my_tex_h = 280;
			float my_tex_w = ImGui::GetWindowWidth();
			ImVec2 pos = ImGui::GetCursorScreenPos();
			auto& io = ImGui::GetIO();
			ImGui::BeginTooltip();
			float region_sz = 32.0f;
			float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
			float region_y = io.MousePos.y - pos.y - region_sz * 0.5f;
			float zoom = 5.0f;
			ImVec2 uv0 = ImVec2((region_x) / my_tex_w, (region_y) / my_tex_h);
			ImVec2 uv1 = ImVec2((region_x + region_sz) / my_tex_w, (region_y + region_sz) / my_tex_h);
			ImGui::Image(m_Preview->GetImGuiTexture(), ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1);
			ImGui::EndTooltip();
		}

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		if (ImGui::Extensions::CheckBox("Vertical Flip", m_Info.bVerticalFlip))
		{
			m_Reload = true;
		}

		if (ImGui::Extensions::CheckBox("Anisotropy", m_Info.bAnisotropyEnable))
		{
			m_Reload = true;
		}

		if (ImGui::Extensions::Combo("Format", "R8_UNORM\0R8G8B8A8\0B8G8R8A8_UNORM\0R16G16B16A16_SFLOAT\0R32G32B32A32_SFLOAT\0", m_FormatFlag))
		{
			m_Info.eFormat = (TextureFormat)m_FormatFlag;
			m_Reload = true;
		}

		if (ImGui::Extensions::Combo("Filter", "NEAREST\0LINEAR\0", m_FilterFlag))
		{
			m_Info.eFilter = (ImageFilter)m_FilterFlag;
			m_Reload = true;
		}

		if (ImGui::Extensions::Combo("Mode", "REPEAT\0MIRRORED_REPEAT\0CLAMP_TO_EDGE\0CLAMP_TO_BORDER\0MIRROR_CLAMP_TO_EDGE\0", m_AddressModeFlag))
		{
			m_Info.eAddressMode = (AddressMode)m_AddressModeFlag;
			m_Reload = true;
		}

		if (ImGui::Extensions::Combo("Border", "FLOAT_TRANSPARENT_BLACK\0INT_TRANSPARENT_BLACK\0FLOAT_OPAQUE_BLACK\0INT_OPAQUE_BLACK\0FLOAT_OPAQUE_WHITE\0INT_OPAQUE_WHITE\0",
			m_BorderColorFlag))
		{
			m_Info.eBorderColor = (BorderColor)m_BorderColorFlag;
			m_Reload = true;
		}
	}

	void TextureInspector::Reset()
	{
		m_FilePath = "";
		m_Titile = "";
		m_BorderColorFlag = 4;
		m_AddressModeFlag = 0;
		m_FormatFlag = 1;
		m_FilterFlag = 1;
		m_Info = {};

		if (m_Preview != nullptr)
		{
			delete m_Preview;
			m_Preview = nullptr;
		}
	}

	void TextureInspector::Open(const std::string& filePath)
	{
		Reset();
		m_Info.Load(filePath);

		m_Preview = new Texture();
		TextureCreateInfo infoCopy = m_Info;
		infoCopy.bImGUIHandle = true;
		Texture::Create(&infoCopy, m_Preview);

		m_FilePath = filePath;
		std::filesystem::path p(m_FilePath);
		m_Titile = p.filename().stem().u8string();

		m_BorderColorFlag = (int)m_Info.eBorderColor;
		m_AddressModeFlag = (int)m_Info.eAddressMode;
		m_FormatFlag = (int)m_Info.eFormat;
		m_FilterFlag = (int)m_Info.eFilter;
	}
}