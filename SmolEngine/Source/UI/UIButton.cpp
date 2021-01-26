#include "stdafx.h"
#include "UIButton.h"

#include "Renderer/Renderer2D.h"
#include "ECS/WorldAdmin.h"
#include "Core/Application.h"
#include "Core/Window.h"

namespace SmolEngine
{
	void UIButton::Init(const std::string& filePath, const std::string& fileName)
	{
		m_TexturePath = filePath;
		m_TetxureName = fileName;
		m_Texture = Texture::Create(filePath);
	}

	void UIButton::SetTexture(const Ref<Texture> texture)
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
		Renderer2D::SubmitSprite({ cameraPos.x + m_UCood.x, cameraPos.y + m_UCood.y, cameraPos.z }, 11, m_Size, 0, m_Texture, 1.0f, m_CurrentColor);
	}

	void UIButton::CalculatePos(const glm::vec2& screenCenter, const float zoomLevel)
	{
#ifdef SMOLENGINE_EDITOR

		float x = fabs((screenCenter.x + (m_UCood.x * 100.0f) - 90.0f));
		float y = fabs((-screenCenter.y + ((m_UCood.y * 100.0f) - ((1.0f * 100.0f) * 0.5f))));

		maxX = static_cast<uint32_t>((x + (m_Size.x * 50.0f))); // why int?
		maxY = static_cast<uint32_t>((y + (m_Size.y * 50.0f)));

		minX = static_cast<uint32_t>((x - (m_Size.x * 50.0f)));
		minY = static_cast<uint32_t>((y - (m_Size.y * 50.0f)));

#else
		auto& app = Application::GetApplication();

		const float width = app.GetWindow().GetHeight();
		const float height = app.GetWindow().GetWidth();
		const float aspectRatio = height / width;
		const float w = 108.0f;

		float x = screenCenter.x + (m_UCood.x * 100.0f);
		float y = fabs((-screenCenter.y + ((m_UCood.y * 100.0f) - ((zoomLevel * aspectRatio) + (zoomLevel * fabs(m_UCood.y) ) ))));

		if (!signbit(m_UCood.y))
		{
			if (m_UCood.y == 0)
			{
				y += w - zoomLevel;
			}
			else
			{
				y += ((m_UCood.y - zoomLevel) * (zoomLevel));

				y += (w + zoomLevel) * (zoomLevel / 5.0f);
			}
		}
		else
		{
			y += ( ( (zoomLevel - m_UCood.y) * (zoomLevel) ) / (m_UCood.y  * 0.5f));


			y += ( (w + zoomLevel) / fabs(m_UCood.y ) * ( (zoomLevel) / ( (zoomLevel / 2.5f) + fabs(m_UCood.y) ) ) );
		}

		if (!signbit(m_UCood.x))
		{
			if (m_UCood.x == 0)
			{
				x -= ((m_UCood.x + zoomLevel) );
			}
			else
			{
				x -= ((fabs(m_UCood.x) + zoomLevel) * (zoomLevel));
			}
		}
		else
		{
			x -= (((zoomLevel + m_UCood.x) * (zoomLevel)) / (m_UCood.x * 0.5f));
		}

		maxX = (x + (m_Size.x * (200.0f / (zoomLevel * aspectRatio )) * 2.0f));
		maxY = (y + (m_Size.y * (200.0f / (zoomLevel * aspectRatio )) * 2.0f));
													
		minX = (x - (m_Size.x * (200.0f / (zoomLevel * aspectRatio )) * 2.0f));
		minY = (y - (m_Size.y * (200.0f / (zoomLevel * aspectRatio )) * 2.0f));

#endif 																				 
	}

	void UIButton::Reload()
	{
		auto& assetMap = WorldAdmin::GetSingleton()->GetActiveScene().GetSceneData().m_AssetMap;

		auto& result = assetMap.find(m_TetxureName);
		if (result != assetMap.end())
		{
			m_Texture = Texture::Create(result->second);
			return;
		}

		NATIVE_ERROR("Button: texture not found, path: {}", m_TexturePath);
	}

	void UIButton::Reset()
	{
		m_CurrentColor = glm::vec4(1.0f);

		m_isHovered = false;
	}

}