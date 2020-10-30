#pragma once

#include "Core/UI/UIElement.h"
#include <cereal/cereal.hpp>

namespace SmolEngine
{
	class Text;

	class UITextLabel : public UIElement
	{
	public:

		UITextLabel();

		void Init(const std::string& fontFilePath, const std::string& fontName);

		// Setters

		void SetText(const std::string& text);

		void SetFont(const std::string& fontFilePath);

		void SetSize(const glm::vec2& size);

		void SetColor(const glm::vec4& color);

		void SetPosition(const glm::vec2& pos);

	private:

		// Internal needs

		void Reload() override;

		/// Rendering

		void Draw(const glm::vec3& cameraPos);

	private:

		glm::vec4 m_Color = glm::vec4(1.0f);

		glm::vec2 m_Position = glm::vec3(1.0f);

		glm::vec2 m_Size = glm::vec2(1.0f);

		///

		std::string m_FontFilePath = "";

		std::string m_FontName = "";

		std::string m_Text = "";

		///

		float m_Padding = 0.0f;

		std::shared_ptr<Text> m_TextLabel = nullptr;

	private:

		friend class cereal::access;

		friend class WorldAdmin;

		friend class UISystem;

		friend class EditorLayer;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(m_ID, m_Type, m_Color.r, m_Color.g, m_Color.b, m_Color.a, m_Position.x, m_Position.y, m_Size.x, m_Size.y, m_FontFilePath, m_FontName, m_Text, m_Padding);
		}

	};
}