#pragma once
#include "Core/Core.h"

#include "Core/Renderer/OpenGL/OpenglContext.h"

namespace SmolEngine 
{
	class GraphicsContext
	{
	public:

		void Setup(GLFWwindow* window);

		void SwapBuffers();

	private:

#ifdef  SMOLENGINE_OPENGL_IMPL

		OpenglContext m_OpenglContext = {};
#else

#endif

	};
}
