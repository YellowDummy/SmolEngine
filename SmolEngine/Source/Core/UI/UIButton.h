#pragma once

#include "Core/UI/UIElement.h"

#include <cereal/cereal.hpp>
#include <glm/glm.hpp>
#include <string>
#include <functional>


namespace SmolEngine
{
	class Event;

	class Texture2D;

	class UIButton: public UIElement
	{
	public:

		UIButton() = default;

		/// Main

		void Init(const std::string& filePath, const std::string& fileName);

		/// Setters

		void SetTexture(const Ref<Texture2D> texture);

		void SetSize(const glm::vec2& size);

		void SetPosition(const glm::vec2& pos);

		void SetOnClickCallback(std::function<void()> callback);

		/// Events

		void OnClick();

		void OnHovered();

		/// Rendering

		void Draw(const glm::vec3& cameraPos);

	private:

		void CalculatePos(const glm::vec2& screenCenter, const float zoomLevel);

		void Reload() override;

		void Reset();

	private:

		glm::vec4 m_CurrentColor = glm::vec4(1.0f);

		glm::vec4 m_HoveredColor = glm::vec4(1.0f);

		glm::vec4 m_PressedColor = glm::vec4(1.0f);

		///

		glm::vec2 m_Size = { 1, 1 };

		glm::vec2 m_UCood = { 0, 0 };

		///

		std::string m_TexturePath = "";

		std::string m_TetxureName = "";

		uint32_t maxX = 0, maxY = 0, minX = 0, minY = 0;

		///

		bool m_isHovered = false;

		bool m_isPressed = false;

		bool m_PressedColorEnabled = false;

		///

		Ref<Texture2D> m_Texture = nullptr;

		std::function<void()> m_ClickCallback = nullptr;

	private:

		friend class cereal::access;

		friend class Scene;

		friend class UISystem;

		friend class EditorLayer;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(m_ID, m_Type, m_TetxureName, m_TexturePath, m_Size.x, m_Size.y, m_UCood.x, m_UCood.y, maxX, maxY, minX, minY, m_isHovered, m_isPressed, m_PressedColorEnabled,
				m_HoveredColor.r, m_HoveredColor.g, m_HoveredColor.b, m_PressedColor.r, m_PressedColor.g, m_PressedColor.b, m_CurrentColor.r, m_CurrentColor.g, m_CurrentColor.b);
		}
	};
}