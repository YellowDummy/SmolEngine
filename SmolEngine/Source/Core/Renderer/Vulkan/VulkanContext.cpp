#include "stdafx.h"
#include "VulkanContext.h"

#include <GLFW/glfw3.h>

namespace SmolEngine
{
	void VulkanContext::OnResize(uint32_t width, uint32_t height)
	{
		if (m_IsInitialized == false)
		{
			return;
		}

		m_Swapchain.OnResize(width, height, &m_CommandBuffer);
	}

	void VulkanContext::Setup(GLFWwindow* window)
	{
		assert(glfwVulkanSupported() == GLFW_TRUE);

		// Vulkan Initialization

		bool swapchain_initialized = false;
		{
			m_Instance.Init();

			m_Device.Init(&m_Instance);

			m_CommandPool.Init(&m_Device);

			swapchain_initialized = m_Swapchain.Init(&m_Instance, &m_Device, window);
			if (swapchain_initialized)
			{
				uint32_t width = 1280, height = 720;
				m_Swapchain.Create(&width, &height);

				m_CommandBuffer.Init(&m_Device, &m_CommandPool, &m_Swapchain);

				m_Semaphore.Init(&m_Device, &m_CommandBuffer);

				m_Swapchain.Prepare();
			}
		}

		if (swapchain_initialized)
		{
			m_IsInitialized = true;
			s_ContextInstance = this;
			m_Window = window;

			return;
		}

		NATIVE_ERROR("Couldn't create Vulkan Context!");
		abort();
	}

	void VulkanContext::BeginFrame()
	{
		// Get next image in the swap chain (back/front buffer)
		VK_CHECK_RESULT(m_Swapchain.AcquireNextImage(m_Semaphore.GetPresentCompleteSemaphore()));
	}

	void VulkanContext::SwapBuffers()
	{
		const auto& present_ref = m_Semaphore.GetPresentCompleteSemaphore();
		const auto& render_ref = m_Semaphore.GetRenderCompleteSemaphore();

		const uint64_t DEFAULT_FENCE_TIME_OUT = 100000000000;

		// Use a fence to wait until the command buffer has finished execution before using it again

		VK_CHECK_RESULT(vkWaitForFences(*m_Device.GetLogicalDevice(), 1, &m_Semaphore.GetVkFences()[m_Swapchain.GetCurrentBufferIndex()], VK_TRUE, UINT64_MAX));
		VK_CHECK_RESULT(vkResetFences(*m_Device.GetLogicalDevice(), 1, &m_Semaphore.GetVkFences()[m_Swapchain.GetCurrentBufferIndex()]));

		// Build Command Buffer

		//BuildTestCommandBuffer(m_CurrentBuffer);

		// Pipeline stage at which the queue submission will wait (via pWaitSemaphores)
		VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		// The submit info structure specifies a command buffer queue submission batch
		VkSubmitInfo submitInfo = {};
		{

			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.pWaitDstStageMask = &waitStageMask;               // Pointer to the list of pipeline stages that the semaphore waits will occur at
			submitInfo.pWaitSemaphores = &present_ref;      // Semaphore(s) to wait upon before the submitted command buffer starts executing
			submitInfo.waitSemaphoreCount = 1;                           // One wait semaphore
			submitInfo.pSignalSemaphores = &render_ref;     // Semaphore(s) to be signaled when command buffers have completed
			submitInfo.signalSemaphoreCount = 1;                         // One signal semaphore
			submitInfo.pCommandBuffers = &m_CommandBuffer.GetCommandBuffer()[m_Swapchain.GetCurrentBufferIndex()]; // Command buffers(s) to execute in this batch (submission)
			submitInfo.commandBufferCount = 1;                           // One command buffer
		}

		// Submit to the graphics queue passing a wait fence
		VK_CHECK_RESULT(vkQueueSubmit(*m_Device.GetQueue(), 1, &submitInfo, m_Semaphore.GetVkFences()[m_Swapchain.GetCurrentBufferIndex()]));

		// Present the current buffer to the swap chain
		// Pass the semaphore signaled by the command buffer submission from the submit info as the wait semaphore for swap chain presentation
		// This ensures that the image is not presented to the windowing system until all commands have been submitted
		VkResult present = m_Swapchain.QueuePresent(*m_Device.GetQueue(), render_ref);
		if (!((present == VK_SUCCESS) || (present == VK_SUBOPTIMAL_KHR))) {

			if (present == VK_ERROR_OUT_OF_DATE_KHR)
			{
				m_Swapchain.OnResize(m_Swapchain.GetWidth(), m_Swapchain.GetHeight(), &m_CommandBuffer);
				return;
			}
			else
			{
				VK_CHECK_RESULT(present);
			}
		}

		VK_CHECK_RESULT(vkWaitForFences(*m_Device.GetLogicalDevice(), 1, &m_Semaphore.GetVkFences()[m_Swapchain.GetCurrentBufferIndex()], VK_TRUE, DEFAULT_FENCE_TIME_OUT));
	}
}