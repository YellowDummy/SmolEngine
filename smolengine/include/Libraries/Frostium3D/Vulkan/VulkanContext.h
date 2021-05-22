#pragma once
#ifndef FROSTIUM_OPENGL_IMPL
#include "Common/Core.h"

#include "Vulkan/VulkanInstance.h"
#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanSwapchain.h"
#include "Vulkan/VulkanCommandBuffer.h"
#include "Vulkan/VulkanSemaphore.h"

struct GLFWwindow;

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	struct GraphicsContextState;

	class VulkanContext
	{
	public:

		VulkanContext() = default;
		~VulkanContext();

		void OnResize(uint32_t* width, uint32_t* height);
		void Setup(GLFWwindow* window, GraphicsContextState* state, uint32_t* width, uint32_t* height);
		void BeginFrame();
		void SwapBuffers(bool skip = false);

		void BeginFrameEx(VkCommandBuffer cmdBuffer);
		void SwapBuffersEx(VkCommandBuffer cmdBuffer);

		// Getters
		inline static VulkanContext* GetSingleton() { return s_ContextInstance; }
		inline GLFWwindow* GetWindow() { return m_Window; }

		inline static VulkanCommandBuffer& GetCommandBuffer() { return m_CommandBuffer; }
		inline static VulkanSwapchain& GetSwapchain() { return m_Swapchain; }
		inline static VulkanInstance& GetInstance() { return m_Instance; }
		inline static VulkanDevice& GetDevice() { return m_Device; }
		inline static VkCommandBuffer GetCurrentVkCmdBuffer() { return m_CurrentVkCmdBuffer; }
		inline static uint64_t GetBufferDeviceAddress(VkBuffer buffer);


	private:
		GLFWwindow*                         m_Window = nullptr;
		GraphicsContextState*               m_ContextState = nullptr;

		inline static VulkanContext*        s_ContextInstance = nullptr;
		inline static VkCommandBuffer       m_CurrentVkCmdBuffer = nullptr;
		inline static VulkanCommandBuffer   m_CommandBuffer = {};
		inline static VulkanSwapchain       m_Swapchain = {};
		inline static VulkanSemaphore       m_Semaphore = {};
		inline static VulkanInstance        m_Instance = {};
		inline static VulkanDevice          m_Device = {};

	private:

		friend class GraphicsContext;
		friend class ImGuiVulkanImpl;
	};
}
#endif
