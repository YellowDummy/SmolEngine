#pragma once
#include "Core/Core.h"

#include <vulkan/vulkan.h>

namespace SmolEngine
{
	class VulkanBuffer
	{
	public:

		VulkanBuffer();

		~VulkanBuffer();

		/// 
		/// Factory
		/// 
		
		static Ref<VulkanBuffer> Create(const void* data, size_t size, VkMemoryPropertyFlags memFlags, VkBufferUsageFlags usageFlags, uint32_t offset = 0,
			VkSharingMode shareMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE);

		/// 
		/// Getters
		/// 

		uint32_t GetSize() const;

		const VkBuffer& GetBuffer() const;

		const VkDeviceMemory& GetDeviceMemory() const;

	private:

		static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags memFlags);

	private:

		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VkDeviceMemory m_DeviceMemory = VK_NULL_HANDLE;

		uint32_t m_MemoryType = UINT32_MAX;
		uint32_t m_Size = UINT32_MAX;

		void* mappedMemory = nullptr;
	};
}