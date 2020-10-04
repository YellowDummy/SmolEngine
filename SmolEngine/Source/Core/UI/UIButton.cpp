#include "stdafx.h"
#include "UIButton.h"

#include "Core/Renderer/Renderer2D.h"
#include "Core/ECS/Scene.h"

namespace SmolEngine
{
	void UIButton::Init(const std::string& filePath, const std::string& fileName)
	{
		m_TexturePath = filePath;
		m_TetxureName = fileName;
		m_Texture = Texture2D::Create(filePath);
	}

	void UIButton::SetTexture(const Ref<Texture2D> texture)
	{
		m_Texture = texture;
	}

	void UIButton::SetSize(const glm::vec2& size)
	{
		m_Size = size;
	}

	void UIButton::SetPosition(const glm::vec2& pos)
	{
		m_UCood = pos;
	}

	void UIButton::OnClick()
	{
		if (m_PressedColorEnabled)
		{
			m_CurrentColor = m_PressedColor;
		}

		if (m_ClickCallback != nullptr)
		{
			m_ClickCallback();
		}
	}

	void UIButton::OnHovered()
	{
		m_isHovered = true;

		m_CurrentColor = m_HoveredColor;
	}

	void UIButton::SetOnClickCallback(std::function<void()> callback)
	{
		m_ClickCallback = callback;
	}

	void UIButton::Draw(const glm::vec3& cameraPos)
	{
		Renderer2D::DrawSprite({ cameraPos.x + m_UCood.x, cameraPos.y + m_UCood.y, cameraPos.z }, m_Size, 0, m_Texture, 1.0f, { m_CurrentColor, 1.0f });
	}

	void UIButton::CalculatePos(const glm::vec2& screenCenter)
	{
		float x = fabs((screenCenter.x + (m_UCood.x * 100.0f) - 90.0f));
		float y = fabs((-screenCenter.y + ((m_UCood.y * 100.0f) - ((1.0f * 100.0f) * 0.5f))));

		maxX = (x + (m_Size.x * 50.0f));
		maxY = (y + (m_Size.y * 50.0f));

		minX = (x - (m_Size.x * 50.0f));
		minY = (y - (m_Size.y * 50.0f));
	}

	void UIButton::Reload()
	{
		auto scene = Scene::GetScene();

		if (scene->PathCheck(m_TexturePath, m_TetxureName))
		{
			m_Texture = Texture2D::Create(m_TexturePath);
		}
	}

	void UIButton::Reset()
	{
		m_CurrentColor = glm::vec3(1.0f);

		m_isHovered = false;
	}

}