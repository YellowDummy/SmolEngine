#include "stdafx.h"
#include "GraphicsContext.h"

namespace SmolEngine
{
	void GraphicsContext::Setup(GLFWwindow* window, uint32_t* width, uint32_t* height)
	{

#ifdef  SMOLENGINE_OPENGL_IMPL

		m_OpenglContext.Setup(window);
#else
		m_VulkanContext.Setup(window, width, height);
#endif

	}

	void GraphicsContext::OnResize(uint32_t height, uint32_t width)
	{
#ifdef  SMOLENGINE_OPENGL_IMPL
#else

		m_VulkanContext.OnResize(height, width);
#endif
	}

	void GraphicsContext::SwapBuffers()
	{

#ifdef  SMOLENGINE_OPENGL_IMPL

		m_OpenglContext.SwapBuffers();
#else

		m_VulkanContext.SwapBuffers();
#endif

	}

	void GraphicsContext::BeginFrame()
	{

#ifdef  SMOLENGINE_OPENGL_IMPL

#else
		m_VulkanContext.BeginFrame();
#endif

	}
}