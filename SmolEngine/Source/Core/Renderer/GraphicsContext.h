#pragma once
#include "Core/Core.h"

#ifdef  SMOLENGINE_OPENGL_IMPL

#include "Core/Renderer/OpenGL/OpenglContext.h"

#else

#include "Core/Renderer/Vulkan/VulkanContext.h"

#endif

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
		VulkanContext m_VulkanContext = {};
#endif

	};
}
