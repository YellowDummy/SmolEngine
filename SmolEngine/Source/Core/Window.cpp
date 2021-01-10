#include "stdafx.h"
#include "Window.h"
#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Events/InputEvent.h"

#include "GLFW/glfw3.h"
#include <glad\glad.h>


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

		WidnowInit(title, width, height);
	}

	void Window::ResizeContext(uint32_t width, uint32_t height)
	{
		if (m_Context != nullptr)
		{
			m_Context->OnResize(width, height);
		}
	}

	void Window::BeginFrame()
	{
		m_Context->BeginFrame();
	}

	void Window::SwapBuffers()
	{
		m_Context->SwapBuffers();
	}

	void Window::ProcessEvents()
	{
		if (!glfwWindowShouldClose(m_Window))
		{
			glfwPollEvents();
		}
	}

	void Window::SetWidth(uint32_t value)
	{
		Data.Width = value;
	}

	void Window::SetHeight(uint32_t value)
	{
		Data.Height = value;
	}

	GLFWwindow* Window::GetNativeWindow() const
	{
		return m_Window;
	}

	GraphicsContext* Window::GetContext() const
	{
		return m_Context;
	}

	uint32_t Window::GetWidth() const
	{
		return Data.Width;
	}

	uint32_t Window::GetHeight() const
	{
		return Data.Height;
	}

	void Window::WidnowInit(const std::string& title, const int& width, const int& height)
	{
		glfwInit();
		glfwSetErrorCallback([](int error, const char* description) { NATIVE_ERROR("GLFW Error ({0}): {1}", error, description); });

#ifndef SMOLENGINE_OPENGL_IMPL

		// No need to create OpenGL window automatically

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

#endif //!SMOLENGINE_OPENGL_IMPL

		// Create Window

		m_Window = glfwCreateWindow((int)width, (int)height, title.c_str(), nullptr, nullptr);
		if (!m_Window)
		{

			NATIVE_ERROR("Failed to create window!");
			assert(m_Window);
			abort();
		}

		// Create Graphics Context

		m_Context = new GraphicsContext();
		m_Context->Setup(m_Window);

		Data.Width = width;
		Data.Height = height;

#ifndef SMOLENGINE_EDITOR

		// Setting Fullscreen

		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		glfwSetWindowMonitor(m_Window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);

#else
		//glfwMaximizeWindow(m_Window);

#endif // SMOLENGINE_OPENGL_IMPL


#ifdef SMOLENGINE_OPENGL_IMPL

		// Setting VSync

		SetVSync(true);

#endif //SMOLENGINE_OPENGL_IMPL

		// Callbacks

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int height, int width)
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