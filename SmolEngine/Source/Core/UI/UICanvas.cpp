#include "stdafx.h"
#include "UICanvas.h"

#include "Core/SLog.h"
#include "Core/EventHandler.h"
#include "Core/Input.h"
#include "Core/Application.h"
#include "Core/Window.h"
#include "Core/Events/MouseEvent.h"
#include "Core/Renderer/Texture.h"
#include "Core/Renderer/Camera.h"


#include <GLFW/glfw3.h>



namespace SmolEngine
{
	void UICanvas::OnUpdate()
	{

	}

	void UICanvas::OnEvent(Event& e)
	{
		S_BIND_EVENT_TYPE(UICanvas, OnMouseClick, EventType::S_MOUSE_PRESS, e);
		S_BIND_EVENT_TYPE(UICanvas, OnMouseMove, EventType::S_MOUSE_MOVE, e);
	}

	Ref<UIElement> UICanvas::AddElement(UIElementType type)
	{
		switch (type)
		{
		case SmolEngine::UIElementType::TextLabel:
		{
			std::shared_ptr<UIElement> element = std::make_shared<UITextLabel>();

			element->m_Type = UIElementType::TextLabel;
			element->m_ID = m_Elements.size();
			m_Elements[element->m_ID] = element;
			return element;

			break;
		}
		case SmolEngine::UIElementType::Button:
		{
			std::shared_ptr<UIElement> element = std::make_shared<UIButton>();

			element->m_Type = UIElementType::Button;
			element->m_ID = m_Elements.size();
			m_Elements[element->m_ID] = element;
			return element;

			break;
		}
		case SmolEngine::UIElementType::TextBox:
		{
			break;
		}
		default:
			break;
		}
	}

	void UICanvas::DeleteElement(size_t id)
	{
		auto result = m_Elements.find(id);
		if (result == m_Elements.end())
		{
			NATIVE_ERROR("Canvas: Element <{}> not found!", id);
			return;
		}

		m_Elements.erase(id);
	}

	void UICanvas::DrawAllElements(const glm::vec3& cameraPos, const float zoomLevel) const
	{
		float w = Application::GetApplication().GetWindowWidth();
		float h = Application::GetApplication().GetWindowHeight();

		glm::vec2 screenCenter = { w / 2.0f, h / 2.5f };

		for (const auto& pair: m_Elements)
		{
			const auto& [key, element] = pair;

			switch (element->m_Type)
			{
			case SmolEngine::UIElementType::TextLabel:
			{
				auto text = std::static_pointer_cast<UITextLabel>(element);
				if (text)
				{
					text->Draw(cameraPos);
				}

				break;
			}
			case SmolEngine::UIElementType::Button:
			{
				auto button = std::static_pointer_cast<UIButton>(element);
				if (button)
				{
					if (button->m_Texture == nullptr) { break; }

					button->CalculatePos(screenCenter, zoomLevel);
					button->Draw(cameraPos);
				}

				break;
			}
			case SmolEngine::UIElementType::TextBox:
			{
				break;
			}
			default:
				break;
			}
		}
	}

	void UICanvas::DrawElement(const size_t id, const glm::vec3& cameraPos, const float zoomLevel) const
	{

		auto result = m_Elements.find(id);
		if (result == m_Elements.end())
		{
			NATIVE_ERROR("Canvas: Element <{}> not found!", id);
			return;
		}

		auto element = result->second;

		float w = Application::GetApplication().GetWindowWidth();
		float h = Application::GetApplication().GetWindowHeight();

		glm::vec2 screenCenter = { h / 2.0f, w / 2.5f };


		switch (element->m_Type)
		{
		case SmolEngine::UIElementType::TextLabel:
		{
			auto text = std::static_pointer_cast<UITextLabel>(element);
			if (text)
			{
				text->Draw(cameraPos);
			}

			break;
		}
		case SmolEngine::UIElementType::Button:
		{
			auto button = std::static_pointer_cast<UIButton>(element);

			if (button->m_Texture == nullptr) { break; }

			button->CalculatePos(screenCenter, zoomLevel);
			button->Draw(cameraPos);

			break;
		}
		case SmolEngine::UIElementType::TextBox:
		{
			break;
		}
		default:
			break;
		}
	}

	Ref<UIButton> UICanvas::GetButton(size_t index) const
	{
		auto element = GetElement(index);
		if (element)
		{
			if (element->m_Type == UIElementType::Button)
			{
				return std::static_pointer_cast<UIButton>(element);
			}
		}

		return nullptr;
	}

	Ref<UITextLabel> UICanvas::GetTextLabel(size_t index) const
	{
		auto element = GetElement(index);
		if (element)
		{
			if (element->m_Type == UIElementType::TextLabel)
			{
				return std::static_pointer_cast<UITextLabel>(element);
			}
		}

		return nullptr;
	}

	Ref<UIElement> UICanvas::GetElement(const size_t id) const
	{
		auto result = m_Elements.find(id);
		if (result == m_Elements.end())
		{
			NATIVE_ERROR("Canvas: Element <{}> not found!", id);
			return nullptr;
		}

		return result->second;
	}

	void UICanvas::ReloadElements()
	{
		for (const auto& pair: m_Elements)
		{
			const auto& [key, element] = pair;
			element->Reload();
		}
	}

	bool UICanvas::OnMouseClick(Event& e)
	{
		const auto win = Application::GetApplication().GetWindow().GetNativeWindow();

		double xpos, ypos;
		glfwGetCursorPos(win, &xpos, &ypos);

		for (const auto& pair : m_Elements)
		{
			const auto& [key, element] = pair;

			if (element->m_Type == UIElementType::Button)
			{
				auto button = std::static_pointer_cast<UIButton>(element);

				if (button)
				{
					if (ypos < button->maxY && ypos > button->minY && xpos < button->maxX && xpos > button->minX)
					{
						button->OnClick();
					}
				}
			}
		}

		return false;
	}

	bool UICanvas::OnMouseMove(Event& e)
	{
		const auto& mouse = static_cast<MouseMoveEvent&>(e);

		for (const auto& pair : m_Elements)
		{
			const auto& [key, element] = pair;

			if (element->m_Type == UIElementType::Button)
			{
				auto button = std::static_pointer_cast<UIButton>(element);

				if (button)
				{
					if (mouse.m_yPos < button->maxY && mouse.m_yPos > button->minY && mouse.m_xPos < button->maxX && mouse.m_xPos > button->minX)
					{
						button->OnHovered();
						continue;
					}

					button->Reset();
				}
			}
		}

		return false;
	}
}