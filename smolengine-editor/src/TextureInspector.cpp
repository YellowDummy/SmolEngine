#include "stdafx.h"
#include "TextureInspector.h"
#include "ImGuiExtension.h"

#include <imgui/imgui.h>

namespace SmolEngine
{
	void TextureInspector::Update()
	{
		ImGui::NewLine();

		if (ImGui::Extensions::CheckBox("Vertical Flip", m_Info.bVerticalFlip))
		{
			m_Info.Save(m_FilePath);
		}

		if (ImGui::Extensions::CheckBox("Anisotropy", m_Info.bAnisotropyEnable))
		{
			m_Info.Save(m_FilePath);
		}

		if (ImGui::Extensions::Combo("Format", "R8_UNORM\0R8G8B8A8\0B8G8R8A8_UNORM\0R16G16B16A16_SFLOAT\0R32G32B32A32_SFLOAT\0", m_FormatFlag))
		{
			m_Info.eFormat = (TextureFormat)m_FormatFlag;
			m_Info.Save(m_FilePath);
		}

		if (ImGui::Extensions::Combo("Filter", "NEAREST\0LINEAR\0", m_FilterFlag))
		{
			m_Info.eFilter = (ImageFilter)m_FilterFlag;
			m_Info.Save(m_FilePath);
		}

		if (ImGui::Extensions::Combo("Mode", "REPEAT\0MIRRORED_REPEAT\0CLAMP_TO_EDGE\0CLAMP_TO_BORDER\0MIRROR_CLAMP_TO_EDGE\0", m_AddressModeFlag))
		{
			m_Info.eAddressMode = (AddressMode)m_AddressModeFlag;
			m_Info.Save(m_FilePath);
		}

		if (ImGui::Extensions::Combo("Border", "FLOAT_TRANSPARENT_BLACK\0INT_TRANSPARENT_BLACK\0FLOAT_OPAQUE_BLACK\0INT_OPAQUE_BLACK\0FLOAT_OPAQUE_WHITE\0INT_OPAQUE_WHITE\0",
			m_BorderColorFlag))
		{
			m_Info.eBorderColor = (BorderColor)m_BorderColorFlag;
			m_Info.Save(m_FilePath);
		}
	}

	void TextureInspector::Reset()
	{
		m_FilePath = "";
		m_BorderColorFlag = 4;
		m_AddressModeFlag = 0;
		m_FormatFlag = 1;
		m_FilterFlag = 1;
		m_Info = {};
	}

	void TextureInspector::Open(const std::string& filePath)
	{
		Reset();
		m_Info.Load(filePath);

		m_FilePath = filePath;
		std::filesystem::path p(m_FilePath);
		m_Titile = "Texture: " + p.filename().stem().u8string();

		m_BorderColorFlag = (int)m_Info.eBorderColor;
		m_AddressModeFlag = (int)m_Info.eAddressMode;
		m_FormatFlag = (int)m_Info.eFormat;
		m_FilterFlag = (int)m_Info.eFilter;
	}
}