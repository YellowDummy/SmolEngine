#pragma once
#include "Core/Core.h"

#include "Core/Renderer/Vulkan/Vulkan.h"
#include "Core/Renderer/Vulkan/VulkanFramebuffer.h"

#include <vector>

struct GLFWwindow;

namespace SmolEngine
{
	class VulkanDevice;

	class VulkanInstance;

	class VulkanCommandBuffer;

	struct DepthStencil
	{
		VkImage Image;
		VkImageView ImageView;
		VkDeviceMemory DeviceMemory;
	};

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

		bool Prepare();


		void Create(uint32_t* width, uint32_t* height, bool vSync = false);

		void OnResize(uint32_t width, uint32_t height, VulkanCommandBuffer* commandBuffer);

		void CleanUp();


		VkResult AcquireNextImage(VkSemaphore presentCompleteSemaphore, uint32_t* imageIndex);

		VkResult QueuePresent(VkQueue queue, uint32_t imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE);

		/// 
		/// Getters
		/// 

		const VulkanFramebuffer& GetFramebuffer() const;

		const VkRenderPass GetRenderPass() const;

		uint32_t GetWidth() const;

		uint32_t GetHeight() const;



	private:

		VkResult CreateFramebuffer(uint32_t width, uint32_t height);

		VkResult InitSurface(GLFWwindow* window);

		VkResult CreateDepthStencil();

		VkResult CreateRenderPass();


		void FindColorSpaceFormat();

	private:

		std::vector<VkImage> m_Images;
		std::vector<SwapchainBuffer> m_Buffers;

		VulkanFramebuffer m_Framebuffer = {};
		DepthStencil m_DepthStencil = {};

		VkRenderPass m_RenderPass = VK_NULL_HANDLE;

		VkFormat m_ColorFormat = VK_FORMAT_B8G8R8A8_UNORM;
		VkFormat m_DepthBufferFormat = VK_FORMAT_B8G8R8A8_UNORM;
		VkColorSpaceKHR m_ColorSpace = VkColorSpaceKHR::VK_COLORSPACE_SRGB_NONLINEAR_KHR;

		VkSwapchainKHR m_Swapchain = nullptr;
		VkPipelineCache m_PipelineCash = nullptr;

		VulkanInstance* m_Instance = nullptr;
		VulkanDevice* m_Device = nullptr;

		VkSurfaceKHR m_Surface = nullptr;
		VkSurfaceFormatKHR* m_SurfaceFormat = nullptr;
		VkPresentModeKHR* m_PresentMode =  nullptr;

		uint32_t m_PresentModeCount = 0;
		uint32_t m_SurfaceFormatCount = 0;
		uint32_t m_ImageCount = 0;
		uint32_t m_Width = 0, m_Height = 0;
		uint32_t m_QueueNodeIndex = UINT32_MAX;

	private:

		friend class VulkanCommandBuffer;

		friend class VulkanFramebuffer;
	};
}