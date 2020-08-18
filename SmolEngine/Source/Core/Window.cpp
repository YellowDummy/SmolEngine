#include "stdafx.h"
#include "Window.h"
#include "Core/Events/ApplicationEvent.h"
#include "Core/Events/MouseEvent.h"
#include "Core/Events/InputEvent.h"

#include "GLFW/glfw3.h"


namespace SmolEngine 
{
	Window::Window(const std::string& title, const int& height, const int& width, Ref<EventHandler> eventHandler)
		:m_Window(nullptr)
	{
		//--------------------------------------------------DATA---------------------------------------------//
		Data.Title = title;
		Data.Height = height;
		Data.Width = width;
		Data.m_eventHandler = eventHandler;
		//--------------------------------------------------DATA---------------------------------------------//

		WidnowInit(title, height, width);
	}

	void Window::OnUpdate()
	{
		if (!glfwWindowShouldClose(m_Window))
		{
			glfwPollEvents();
			m_Context->SwapBuffers();
		}
	}

	void Window::SetWidth(int value)
	{
		Data.Width = value;
	}

	void Window::SetHeight(int value)
	{
		Data.Height = value;
	}

	unsigned int Window::GetWidth() const
	{
		return Data.Width;
	}

	unsigned int Window::GetHeight() const
	{
		return Data.Height;
	}

	void Window::WidnowInit(const std::string& title, const int& height, const int& width)
	{
		glfwInit();
		glfwSetErrorCallback([](int error, const char* description) { NATIVE_ERROR("GLFW Error ({0}): {1}", error, description); });

		m_Window = glfwCreateWindow((int)width, (int)height, title.c_str(), nullptr, nullptr);
		if (!m_Window)
		{
			return;
		}

		m_Context = new OpenglContext(m_Window);
		m_Context->Setup();

		SetVSync(true);
		//-----------------------------------------------CALLBACKS-----------------------------------------//

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
				Data.Height = height;
				Data.Width = width;

				WindowResizeEvent resizeEvent(Data);
				Data.m_eventHandler->SendEvent(resizeEvent, EventType::S_WINDOW_RESIZE, EventCategory::S_EVENT_APPLICATION);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) 
		{
				WindowCloseEvent closeEvent;
				Data.m_eventHandler->SendEvent(closeEvent, EventType::S_WINDOW_CLOSE, EventCategory::S_EVENT_APPLICATION);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow * window, int key, int scancode, int action, int mods)
		{
				KeyEvent keyEvent;

				switch (action)
				{
				case GLFW_PRESS:
				{
					Data.m_eventHandler->SendEvent(keyEvent, EventType::S_KEY_PRESS, EventCategory::S_EVENT_KEYBOARD, action, key);
					break;
				}
				case GLFW_RELEASE:
				{
					Data.m_eventHandler->SendEvent(keyEvent, EventType::S_KEY_RELEASE, EventCategory::S_EVENT_KEYBOARD, action, key);
					break;
				}
				case GLFW_REPEAT:
				{
					Data.m_eventHandler->SendEvent(keyEvent, EventType::S_KEY_REPEAT, EventCategory::S_EVENT_KEYBOARD, action, key);
					break;
				}
				default:
					break;
				}
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) 
		{
				MouseButtonEvent mouseButton(button);

				switch (action)
				{
				case GLFW_PRESS:
				{
					Data.m_eventHandler->SendEvent(mouseButton, EventType::S_MOUSE_PRESS, EventCategory::S_EVENT_MOUSE, action, button);
					break;
				}
				case GLFW_RELEASE:
				{
					Data.m_eventHandler->SendEvent(mouseButton, EventType::S_MOUSE_RELEASE, EventCategory::S_EVENT_MOUSE, action, button);
					break;
				}
				}
				
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
				MouseScrollEvent scrollEvent(xOffset, yOffset);
				Data.m_eventHandler->SendEvent(scrollEvent, EventType::S_MOUSE_SCROLL, EventCategory::S_EVENT_MOUSE);
		});
	

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) 
		{
				MouseMoveEvent mouseEvent(xPos, yPos);
				Data.m_eventHandler->SendEvent(mouseEvent, EventType::S_MOUSE_MOVE, EventCategory::S_EVENT_MOUSE);
		});

		//-----------------------------------------------CALLBACKS-----------------------------------------//

	}

	void Window::ShutDown()
	{
		glfwDestroyWindow(m_Window);
	}

	void Window::SetVSync(bool enabled)
	{
		NATIVE_INFO("VSync enabled: {0}", enabled);
		enabled ? glfwSwapInterval(1) : glfwSwapInterval(0);
	}


}