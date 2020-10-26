#include "stdafx.h"
#include "UISystem.h"
#include "Core/Window.h"
#include "Core/Application.h"
#include "Core/Events/MouseEvent.h"

#include "Core/ECS/Components/CanvasComponent.h"

#include <GLFW/glfw3.h>

namespace SmolEngine
{
	void UISystem::OnUpdate(CanvasComponent& canvas)
	{

	}

	void UISystem::OnEvent(CanvasComponent& canvas, Event& e)
	{
		if (e.m_EventType == (int)EventType::S_MOUSE_PRESS)
		{
			OnMouseClick(canvas, e);
		}

		if (e.m_EventType == (int)EventType::S_MOUSE_MOVE)
		{
			OnMouseMove(canvas, e);
		}
	}

	Ref<UIElement> UISystem::AddElement(CanvasComponent& canvas, UIElementType type)
	{
		switch (type)
		{
		case SmolEngine::UIElementType::TextLabel:
		{
			std::shared_ptr<UIElement> element = std::make_shared<UITextLabel>();

			element->m_Type = UIElementType::TextLabel;
			element->m_ID = canvas.Elements.size();
			canvas.Elements[element->m_ID] = element;
			return element;
		}
		case SmolEngine::UIElementType::Button:
		{
			std::shared_ptr<UIElement> element = std::make_shared<UIButton>();

			element->m_Type = UIElementType::Button;
			element->m_ID = canvas.Elements.size();
			canvas.Elements[element->m_ID] = element;
			return element;
		}
		case SmolEngine::UIElementType::TextBox:
		{
			return nullptr;
		}
		default:
			return nullptr;
		}
	}

	bool UISystem::DeleteElement(CanvasComponent& canvas, size_t id)
	{
		return canvas.Elements.erase(id);
	}

	void UISystem::DrawAllElements(const CanvasComponent& canvas, const glm::vec3& cameraPos, const float zoomLevel)
	{
		float w = Application::GetApplication().GetWindowWidth();
		float h = Application::GetApplication().GetWindowHeight();

		glm::vec2 screenCenter = { w / 2.0f, h / 2.5f };

		for (const auto& pair : canvas.Elements)
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

	Ref<UIButton> UISystem::GetButton(const CanvasComponent& canvas, size_t index)
	{
		auto element = GetElement(canvas, index);
		if (element)
		{
			if (element->m_Type == UIElementType::Button)
			{
				return std::static_pointer_cast<UIButton>(element);
			}
		}

		return nullptr;
	}

	Ref<UITextLabel> UISystem::GetTextLabel(const CanvasComponent& canvas, size_t index)
	{
		auto element = GetElement(canvas, index);
		if (element)
		{
			if (element->m_Type == UIElementType::TextLabel)
			{
				return std::static_pointer_cast<UITextLabel>(element);
			}
		}

		return nullptr;
	}

	Ref<UIElement> UISystem::GetElement(const CanvasComponent& canvas, const size_t index)
	{
		auto result = canvas.Elements.find(index);
		if (result == canvas.Elements.end())
		{
			NATIVE_ERROR("Canvas: Element <{}> not found!", index);
			return nullptr;
		}

		return result->second;
	}

	void UISystem::ReloadElements(CanvasComponent& canvas)
	{
		for (const auto& pair : canvas.Elements)
		{
			const auto& [key, element] = pair;

			element->Reload();
		}
	}

	bool UISystem::OnMouseClick(CanvasComponent& canvas, Event& e)
	{
		const auto win = Application::GetApplication().GetWindow().GetNativeWindow();

		double xpos, ypos;
		glfwGetCursorPos(win, &xpos, &ypos);

		for (const auto& pair : canvas.Elements)
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

	bool UISystem::OnMouseMove(CanvasComponent& canvas, Event& e)
	{
		const auto& mouse = static_cast<MouseMoveEvent&>(e);

		for (const auto& pair : canvas.Elements)
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