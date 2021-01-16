#include "stdafx.h"
#include "Text.h"

#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"
#include "Core/SLog.h"


namespace SmolEngine
{
	Ref<Text> Text::Create(const std::string& filePath)
	{
		return std::make_shared<Text>(filePath);
	}

	Text::Text(const std::string& fontFilePath)
	{
		FT_Error error;
		error = FT_Init_FreeType(&m_Library);
		if (error)
		{
			NATIVE_ERROR("Couldn't init FreeType2");
			abort();
		}

		SetFont(fontFilePath);
	}

	Text::~Text()
	{
		FT_Done_Face(m_Face);
		FT_Done_FreeType(m_Library);
	}

	void Text::SetText(const std::string& text)
	{
        if (text.empty()) { return; }

        m_Text = text;

        m_Characters.clear();
        m_Characters.reserve(text.size());
        FT_Error error;

        for (auto c : text)
        {
            error = FT_Set_Char_Size(m_Face, 0, (int)(m_FontSize * 64), 300, 300);
            if (error)
            {
                NATIVE_ERROR("Couldn't set font size");
                abort();
            }

            FT_UInt glyph_index = FT_Get_Char_Index(m_Face, c);

            error = FT_Load_Glyph(m_Face, glyph_index, FT_LOAD_DEFAULT);
            if (error)
            {
                NATIVE_ERROR("Couldn't load glyph");
                abort();
            }

            error = FT_Render_Glyph(m_Face->glyph, FT_RENDER_MODE_NORMAL);
            if (error)
            {
                NATIVE_ERROR("Couldn't render glyph");
                abort();
            }

            Character character;
            character.Size = { m_Face->glyph->bitmap.width, m_Face->glyph->bitmap_top };
            character.Texture = Texture::Create(&m_Face->glyph->bitmap);

            if (c == ' ')
            {
                character.Skip = true;
            }

            m_Characters.push_back(character);
        }
	}

	void Text::SetFont(const std::string& fontFilePath)
	{
        FT_Error error;
        error = FT_New_Face(m_Library, fontFilePath.c_str(), 0, &m_Face);

        if (error == FT_Err_Unknown_File_Format)
        {
            NATIVE_ERROR("Couldn't open font file, unknown file format");
            abort();
        }
        else if (error)
        {
            NATIVE_ERROR("Couldn't open font file");
            abort();
        }

        SetText(m_Text);
	}

	void Text::Render(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color, float padding)
	{
        if (m_Characters.empty()) { return; }

        m_xOffset = 0.0f;

        for (auto character : m_Characters)
        {
            auto x = scale.x + (character.Size.x / 800.0f);
            auto y = scale.y + (character.Size.y / 8000.0f);

            if (!character.Skip)
            {
                Renderer2D::DrawUIText({ position.x + m_xOffset, position.y, position.z }, { x, y }, character.Texture, color);
                m_xOffset += x + (y / 8) + padding;
            }
            else
            {
                m_xOffset += x + (y / 8) + padding;
            }
        }
	}
}