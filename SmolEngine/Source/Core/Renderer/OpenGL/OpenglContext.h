#pragma once
#include "Core/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace SmolEngine 
{
	class OpenglContext: public GraphicsContext
	{
	public:

		OpenglContext(GLFWwindow* window);

		void Setup() override;

		void SwapBuffers() override;

		inline GLFWwindow* GetWindow() { return m_Window; }

	private:

		GLFWwindow* m_Window;
	};
}

