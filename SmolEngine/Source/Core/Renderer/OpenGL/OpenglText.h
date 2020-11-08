#pragma once
#include "Core/Core.h"
#include "Core/Renderer/Text.h"

#include <vector>

extern "C" {
#include <ft2build.h>
#include FT_FREETYPE_H
}

//TODO: Add Texture Atlas

namespace SmolEngine
{
	class Texture2D;

	class OpenglText: public Text
	{
	public:

		OpenglText(const std::string& fontFilePath);

		~OpenglText();

		/// Setters

		void SetText(const std::string& text) override;

		void SetFont(const std::string& fontFilePath) override;

	private:

		/// Rendering

		void Render(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color, float padding) override;

	private:

		struct Character
		{
			Ref<Texture2D> Texture = nullptr;
			glm::vec2 Size = { 0, 0 };
			bool Skip = false;
		};

		FT_Face m_Face;
		FT_Library m_Library;

		float m_xOffset = 0.0f;
		int m_FontSize = 100.0f;

		std::string m_Text = "";

		std::vector<Character> m_Characters;
	};
}