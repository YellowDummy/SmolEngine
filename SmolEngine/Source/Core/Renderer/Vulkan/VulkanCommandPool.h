#pragma once
#include "Core/Core.h"

#include <vulkan/vulkan.h>

namespace SmolEngine
{
	class VulkanDevice;

	class VulkanCommandPool
	{
	public:

		VulkanCommandPool();

		~VulkanCommandPool();

		/// 
		/// Main
		///
		
		void Init(VulkanDevice* device);

	private:

		bool SetupCommandPool(const VulkanDevice* device);

	public:

		/// 
		/// Getters
		/// 

		const VkCommandPool* GetCommandPool();

	private:

		VkCommandPool m_VkCommandPool = VK_NULL_HANDLE;

		VulkanDevice* m_Device = nullptr;

	private:

		friend class VulkanRendererAPI;
	};
}