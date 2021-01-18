#pragma once
#include "Core/Core.h"

#ifdef  SMOLENGINE_OPENGL_IMPL
#include "Renderer/OpenGL/OpenglContext.h"
#else
#include "Renderer/Vulkan/VulkanContext.h"
#endif

namespace SmolEngine 
{
	class GraphicsContext
	{
	public:

		/// Main
		
		void OnResize(uint32_t width, uint32_t height);

		void Setup(GLFWwindow* window, uint32_t* width, uint32_t* height);

		void SwapBuffers();

		void BeginFrame();

		/// Getters

#ifndef  SMOLENGINE_OPENGL_IMPL

		VulkanContext& GetVulkanContext() { return m_VulkanContext; }
#endif

	private:

#ifdef  SMOLENGINE_OPENGL_IMPL

		OpenglContext m_OpenglContext = {};
#else
		VulkanContext m_VulkanContext = {};
#endif

	};
}
