#pragma once
#include "Common/Core.h"
#include "Common/Texture.h"
#include "Common/VertexBuffer.h"
#include "Common/IndexBuffer.h"

#include <array>
#include <string>
#include <glm/glm.hpp>

namespace Frostium
{
	class Text
	{
	public:

		// Factory
		static Ref<Text> CreateSDF(const std::string& sdfPath, const std::string& texturePath);
		static void CreateSDF(const std::string& sdfPath, const std::string& texturePath, Text* out_text);

		// TODO: add width, height, offset etc
		// Setters
		void SetText(const std::string& text);
		void SetColor(const glm::vec4& color);
		void SetPosition(const glm::vec2& pos);
		void SetScale(const glm::vec2& scale);
		void SetRotation(float rot);
		void SetSize(float size);

		// Getters
		const std::string& GetText() const;

	private:

		// Helpers
		int32_t NextValuePair(std::stringstream* stream);
		void GenerateText(const std::string& text);

	private:

		struct bmchar {
			uint32_t x, y;
			uint32_t width;
			uint32_t height;
			int32_t xoffset;
			int32_t yoffset;
			int32_t xadvance;
			uint32_t page;
		};

		bool                    m_Initialized = false;
		uint32_t                m_IndexCount = 0;
		float                   m_Size = 36.0f;
		glm::vec4               m_Color = glm::vec4(1.0f);
		glm::vec3               m_Pos = glm::vec3(0.0f);
		glm::vec3               m_Scale = glm::vec3(1.0f);
		glm::vec3               m_Rotation = glm::vec3(0.0f);
		Texture                 m_SDFTexture = {};
		VertexBuffer            m_VertexBuffer = {};
		IndexBuffer             m_IndexBuffer = {};
		std::string             m_Text = "";
		std::array<bmchar, 255> m_FontChars;

	private:

		friend class Renderer2D;
	};
}