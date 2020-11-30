#include "stdafx.h"
#include "VulkanContext.h"

#include <GLFW/glfw3.h>

namespace SmolEngine
{
	void VulkanContext::Setup(GLFWwindow* window)
	{
		assert(glfwVulkanSupported() == GLFW_TRUE);

		m_Window = window;
		bool swapchain_initialized = false;

		// Vulkan Initialization
		{
			m_Instance.Init();

			m_Device.Init(&m_Instance);

			m_CommandPool.Init(&m_Device);

			swapchain_initialized = m_Swapchain.Init(&m_Instance, &m_Device, m_Window);
			if (swapchain_initialized)
			{
				uint32_t width = 1280, height = 720;
				m_Swapchain.Create(&width, &height);

				m_CommandBuffer.Init(&m_Device, &m_CommandPool, &m_Swapchain);
			}
		}

		if (swapchain_initialized)
		{
			glfwMakeContextCurrent(m_Window);




			return;
		}

		NATIVE_ERROR("Couldn't create Vulkan Context!");
		abort();
	}

	void VulkanContext::SwapBuffers()
	{
		glfwSwapBuffers(m_Window);
	}
}