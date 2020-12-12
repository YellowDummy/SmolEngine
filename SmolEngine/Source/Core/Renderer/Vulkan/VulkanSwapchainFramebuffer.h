#pragma once
#include "Core/Renderer/Vulkan/Vulkan.h"

namespace SmolEngine
{
	class VulkanSwapchain;

	class VulkanSwapchainFramebuffer
	{
	public:

		VulkanSwapchainFramebuffer();

		~VulkanSwapchainFramebuffer();

		///
		/// Main
		/// 
		
		VkResult Init(VulkanSwapchain* swapchain, uint32_t width, uint32_t height);

		VkResult Create(uint32_t width, uint32_t height);


		void Clear();

		///
		/// Getters
		///

		const std::vector<VkFramebuffer>& GetVkFramebuffers() const;

	private:

		std::vector<VkFramebuffer> m_Framebuffers;
		VulkanSwapchain* m_Swapchain = nullptr;

	private:
		friend class VulkanSwapchain;
	};
}