#include "stdafx.h"
#include "GraphicsContext.h"

namespace SmolEngine
{
	void GraphicsContext::Setup(GLFWwindow* window)
	{

#ifdef  SMOLENGINE_OPENGL_IMPL

		m_OpenglContext.Setup(window);
#else

#endif

	}

	void GraphicsContext::SwapBuffers()
	{

#ifdef  SMOLENGINE_OPENGL_IMPL

		m_OpenglContext.SwapBuffers();
#else

#endif

	}
}