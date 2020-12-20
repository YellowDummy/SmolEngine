#pragma once
#include "Core/Core.h"

#include "Core/Renderer/Vulkan/Vulkan.h"

namespace SmolEngine
{
	class VulkanDevice;

	class VulkanBuffer
	{
	public:

		VulkanBuffer();

		~VulkanBuffer();

		/// 
		/// Main
		/// 
		
		void Create(const void* data, size_t size, VkMemoryPropertyFlags memFlags, VkBufferUsageFlags usageFlags, uint32_t offset = 0,
			VkSharingMode shareMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE);

		void Create(size_t size, VkMemoryPropertyFlags memFlags, VkBufferUsageFlags usageFlags, uint32_t offset = 0,
			VkSharingMode shareMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE);

		void SetData(const void* data, size_t size, uint32_t offset = 0);

		void UpdateData(const void* data, size_t size, uint32_t offset = 0);

		void* MapMemory();

		void UnMapMemory();

		void Destroy();

		/// 
		/// Getters
		/// 

		uint32_t GetSize() const;

		const VkBuffer& GetBuffer() const;

		const VkDeviceMemory GetDeviceMemory() const;

	private:

		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags memFlags);

	private:

		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VkDeviceMemory m_DeviceMemory = VK_NULL_HANDLE;
		VkDeviceSize m_MemoryRequirementsSize = 0;

		uint32_t m_MemoryType = UINT32_MAX;
		uint32_t m_Size = UINT32_MAX;

		void* m_Mapped = nullptr;
		VulkanDevice* m_Device = nullptr;
	};
}