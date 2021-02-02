#pragma once
#include "Core/Core.h"

#include "Renderer/Vulkan/Vulkan.h"
#include "Renderer/Vulkan/VulkanFramebuffer.h"

#include <vector>
#include <glm/glm.hpp>

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

		/// Init

		bool Init(VulkanInstance* instance, VulkanDevice* device, GLFWwindow* window);

		bool Prepare(uint32_t width, uint32_t height);

		void Create(uint32_t* width, uint32_t* height, bool vSync = false);

		void CleanUp();

		void ClearColors(VkCommandBuffer cmdBuffer, const glm::vec4& color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

		/// Main

		VkResult AcquireNextImage(VkSemaphore presentCompleteSemaphore);

		VkResult QueuePresent(VkQueue queue, VkSemaphore waitSemaphore = VK_NULL_HANDLE);

		/// Events

		void OnResize(uint32_t width, uint32_t height, VulkanCommandBuffer* commandBuffer);

		/// Getters

		const VkFramebuffer GetCurrentFramebuffer() const;

		const VkSwapchainKHR& GetVkSwapchain() const;

		const VkRenderPass GetVkRenderPass() const;

		const VkSurfaceKHR GetVkSurface() const;

		const VkFormat& GetColorFormat() const;

		const VkFormat& GetDepthFormat() const;

		uint32_t GetCurrentBufferIndex() const;

		uint32_t& GetCurrentBufferIndexRef();

		uint32_t GetHeight() const;

		uint32_t GetWidth() const;

	private:

		VkResult CreateFramebuffers(uint32_t width, uint32_t height);

		VkResult InitSurface(GLFWwindow* window);

		VkResult CreateRenderPass();

		VkResult CreateDepthStencil();

		void GetPtrs();

		void FindColorSpaceFormat();

		void FindDepthStencilFormat();

		void FreeResources();

	private:

		DepthStencil                       m_DepthStencil = {};
		VkClearAttachment                  m_ClearAttachments[2] = {};

		VkFormat                           m_ColorFormat = VK_FORMAT_B8G8R8A8_UNORM;
		VkFormat                           m_DepthBufferFormat = VK_FORMAT_B8G8R8A8_UNORM;
		VkColorSpaceKHR                    m_ColorSpace = VkColorSpaceKHR::VK_COLORSPACE_SRGB_NONLINEAR_KHR;

		VkRenderPass                       m_RenderPass = nullptr;
		VkSwapchainKHR                     m_Swapchain = nullptr;
		VkPipelineCache                    m_PipelineCash = nullptr;
		VulkanInstance*                    m_Instance = nullptr;
		VulkanDevice*                      m_Device = nullptr;
		VkSurfaceKHR                       m_Surface = nullptr;
		VkSurfaceFormatKHR*                m_SurfaceFormat = nullptr;
		VkPresentModeKHR*                  m_PresentMode =  nullptr;

		uint32_t                           m_PresentModeCount = 0;
		uint32_t                           m_SurfaceFormatCount = 0;
		uint32_t                           m_ImageCount = 0;
		uint32_t                           m_Width = 0, m_Height = 0;
		uint32_t                           m_QueueNodeIndex = UINT32_MAX;
		uint32_t                           m_CurrentBufferIndex = 0;

		std::vector<VkImage>               m_Images;
		std::vector<VkFramebuffer>         m_Framebuffers;
		std::vector<SwapchainBuffer>       m_Buffers;

	private:

		friend class VulkanCommandBuffer;
		friend class VulkanFramebuffer;
		friend class VulkanSwapchainFramebuffer;
	};
}