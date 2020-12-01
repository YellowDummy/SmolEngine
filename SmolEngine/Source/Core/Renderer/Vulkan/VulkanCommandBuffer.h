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
		
		bool Init(VulkanDevice* device, VulkanCommandPool* commandPool, VulkanSwapchain* targetSwapchain);

		bool Recrate();

		/// 
		/// Getters
		/// 

		const std::vector<VkCommandBuffer>& GetCommandBuffer() const;

		size_t GetBufferSize() const;

	private:

		std::vector<VkCommandBuffer> m_CommandBuffers;


		VulkanSwapchain* m_TargetSwapchain = nullptr;

		VulkanCommandPool* m_CommandPool = nullptr;

		VulkanDevice* m_Device = nullptr;
	};
}