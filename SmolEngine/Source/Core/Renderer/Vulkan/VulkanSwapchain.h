#pragma once
#include "Core/Core.h"

#include "Core/Renderer/Vulkan/Vulkan.h"
#include <vector>

struct GLFWwindow;

namespace SmolEngine
{
	class VulkanDevice;

	class VulkanInstance;

	struct SwapchainBuffer
	{
		VkImage Image;
		VkImageView View;
	};

	class VulkanSwapchain
	{
	public:

		VulkanSwapchain();

		~VulkanSwapchain();

		/// 
		/// Main
		/// 
		
		bool Init(VulkanInstance* instance, VulkanDevice* device, GLFWwindow* window);

		void Create(uint32_t* width, uint32_t* height, bool vSync = false);

		void CleanUp();


		VkResult AcquireNextImage(VkSemaphore presentCompleteSemaphore, uint32_t* imageIndex);

		VkResult QueuePresent(VkQueue queue, uint32_t imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE);

	private:

		VkResult InitSurface(GLFWwindow* window);

		void FindColorSpaceFormat();

	private:

		VkFormat m_ColorFormat;
		VkColorSpaceKHR m_ColorSpace;
		VkSwapchainKHR m_Swapchain;

		VulkanInstance* m_Instance = nullptr;
		VulkanDevice* m_Device = nullptr;

		std::vector<VkImage> m_Images;
		std::vector<SwapchainBuffer> m_Buffers;

		VkSurfaceKHR m_Surface = nullptr;
		VkSurfaceFormatKHR* m_SurfaceFormat = nullptr;
		VkPresentModeKHR* m_PresentMode;

		uint32_t m_PresentModeCount = 0;
		uint32_t m_SurfaceFormatCount = 0;
		uint32_t m_ImageCount = 0;
		uint32_t m_Width = 0, m_Height = 0;
		uint32_t m_QueueNodeIndex = UINT32_MAX;

	private:

		friend class VulkanCommandBuffer;
	};
}