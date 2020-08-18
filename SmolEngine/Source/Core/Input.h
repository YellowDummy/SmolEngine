#pragma once

#include "../Libraries/glfw/include/GLFW/glfw3.h"
#include "Core/EventHandler.h"
#include "InputCodes.h"
#include "Core/SLog.h"
#include "Core/Application.h"
#include "Core/Window.h"

#include <memory>
#include <functional>

namespace SmolEngine 
{
	class Input
	{
	public:
		Input() {}

		inline static bool IsKeyPressed(KeyCode key)
		{
			auto window = static_cast<GLFWwindow*>(Application::GetApplication().GetWindow().GetNativeWindow());
			auto state = glfwGetKey(window, static_cast<int32_t>(key));
			return state == GLFW_PRESS || state == GLFW_REPEAT;
		}

		inline static bool IsMouseButtonPressed(MouseCode button)
		{
			auto window = static_cast<GLFWwindow*>(Application::GetApplication().GetWindow().GetNativeWindow());
			auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
			return state == GLFW_PRESS;
		}

		inline static float Input::GetMouseX()
		{
			auto [x, y] = GetMousePosition();
			return x;
		}

		inline static float Input::GetMouseY()
		{
			auto [x, y] = GetMousePosition();
			return y;
		}

		template<typename className>
		inline static void SetEventCallback(std::_Binder<std::remove_cv<std::_Unforced>::type, 
			bool(className::*)(Event& eventRef), className*, const std::_Ph<1>& >& callback, SmolEngine::EventType type, Event& event)
		{
			event.m_Handled = callback(event);
		}

		template<typename className>
		inline static void SetEventCategoryCallback(std::_Binder<std::remove_cv<std::_Unforced>::type, 
			bool(className::*)(Event& eventRef), className*, const std::_Ph<1>& >& callback, EventCategory category, Event& event)
		{
			event.m_Handled = callback(event);
		}

		inline static bool IsEventReceived(SmolEngine::EventType type, Event& event)
		{
			return event.m_EventType == (unsigned int)type;
		}

	private:

		inline static std::pair<float, float> Input::GetMousePosition()
		{
			auto window = static_cast<GLFWwindow*>(Application::GetApplication().GetWindow().GetNativeWindow());
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);

			return { (float)xpos, (float)ypos };
		}
	};

}

#define S_BIND_EVENT_CATEGORY(className, Callback, EventType, Event) if(Event.m_EventCategory == (unsigned int)EventType) {SmolEngine::Input::SetEventCategoryCallback<className>(std::bind(&className::Callback, this, std::placeholders::_1), EventType, Event); }
#define S_BIND_EVENT_TYPE(className, Callback, EventType, Event) if(Event.m_EventType == (unsigned int)EventType) { SmolEngine::Input::SetEventCallback<className>(std::bind(&className::Callback, this, std::placeholders::_1), EventType, Event); }