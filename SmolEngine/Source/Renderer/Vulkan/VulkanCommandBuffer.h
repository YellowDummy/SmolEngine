#pragma once
#include "Core/Core.h"
#include "Renderer/Vulkan/Vulkan.h"

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

		/// Main
		
		bool Init(VulkanDevice* device, VulkanCommandPool* commandPool, VulkanSwapchain* targetSwapchain);

		bool Recrate();


		static void EndSingleCommandBuffer(const VkCommandBuffer cmdBuffer);

		static void FlushCommandBuffer(const VkCommandBuffer cmdBuffer);


		static const VkCommandBuffer CreateSingleCommandBuffer(bool oneCommand = true);

		/// Getters

		const VkCommandBuffer GetVkCommandBuffer() const;

		size_t GetBufferSize() const;

		std::vector<VkCommandBuffer> m_CommandBuffers;

	private:


		VulkanSwapchain*           m_TargetSwapchain = nullptr;
		VulkanCommandPool*         m_CommandPool = nullptr;
		VulkanDevice*              m_Device = nullptr;
	};
}