#pragma once
#ifndef FROSTIUM_OPENGL_IMPL
#include "Vulkan/Vulkan.h"

namespace Frostium
{
	class VulkanDevice;

	class VulkanCommandPool
	{
	public:

		VulkanCommandPool();
		~VulkanCommandPool();

		void Init(VulkanDevice* device);
		void Reset();

	private:

		bool SetupCommandPool(const VulkanDevice* device);

	public:

		// Getters
		const VkCommandPool GetCommandPool() const;

	private:

		VkCommandPool m_VkCommandPool = VK_NULL_HANDLE;
		VulkanDevice* m_Device = nullptr;

	private:

		friend class VulkanRendererAPI;
	};
}
#endif