#include "stdafx.h"
#include "Window.h"

namespace SmolEngine 
{
	void Window::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	inline unsigned int Window::GetWidth() const
	{
		return Data.Width;
	}

	inline unsigned int Window::GetHeight() const
	{
		return Data.Height;
	}

}