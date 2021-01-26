#pragma once
#include "Core/Core.h"

#include <vector>
#include <string>
#include <glm/glm.hpp>

extern "C" {
#include <ft2build.h>
#include FT_FREETYPE_H
}

namespace SmolEngine
{
	class Texture;

	class Text
	{
	public:

		Text(const std::string& fontFilePath);

		~Text();

		///  Setters

		void SetText(const std::string& text);

		void SetFont(const std::string& fontFilePath);

		/// Factory

		static Ref<Text> Create(const std::string& filePath);

	private:

		///  Rendering

		void Render(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color, float padding);

	private:

		struct Character
		{
			Ref<Texture> Texture = nullptr;
			glm::vec2 Size = { 0, 0 };
			bool Skip = false;
		};

		FT_Face m_Face;
		FT_Library m_Library;

		float m_xOffset = 0.0f;
		int m_FontSize = 100;

		std::string m_Text = "";
		std::vector<Character> m_Characters;

	private:

		friend class UITextLabel;
	};
}