#pragma once
#include "Core/Core.h"
#include "Core/Renderer/Vulkan/Vulkan.h"

namespace SmolEngine
{
	class VulkanDevice;

	class VulkanSwapchain;

	class VulkanCommandPool;

	class VulkanCommandBuffer
	{
	public:

		VulkanCommandBuffer();

		~VulkanCommandBuffer();

		/// 
		/// Main
		/// 
		
		bool Init(const VulkanDevice* device, const VulkanCommandPool* commandPool, const VulkanSwapchain* targetSwapchain);

	private:

		std::vector<VkCommandBuffer> m_CommandBuffers;
	};
}