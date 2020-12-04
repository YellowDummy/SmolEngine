#include "stdafx.h"
#include "VulkanBuffer.h"

#include "Core/Renderer/Vulkan/VulkanContext.h"

namespace SmolEngine
{
	VulkanBuffer::VulkanBuffer()
	{
		m_Device = &VulkanContext::GetDevice();
	}

	VulkanBuffer::~VulkanBuffer()
	{

#if 0
		const auto& device = *m_Device->GetLogicalDevice();
		vkUnmapMemory(device, m_DeviceMemory);
		vkDestroyBuffer(device, m_Buffer, nullptr);
		vkFreeMemory(device, m_DeviceMemory, nullptr);
#endif
	}

	void VulkanBuffer::Create(const void* data, size_t size, VkMemoryPropertyFlags memFlags, VkBufferUsageFlags usageFlags, uint32_t offset, VkSharingMode shareMode)
	{
		const auto& device = *m_Device->GetLogicalDevice();

		VkDeviceSize bufferSize = size;
		VkBufferCreateInfo bufferInfo = {};
		{
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = bufferSize;
			bufferInfo.usage = usageFlags;
			bufferInfo.sharingMode = shareMode;

			VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, &m_Buffer);
			assert(result == VK_SUCCESS);
		}

		assert(m_Buffer != VK_NULL_HANDLE);

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(device, m_Buffer, &memoryRequirements);
		m_MemoryType = FindMemoryType(memoryRequirements.memoryTypeBits, memFlags);
		m_MemoryRequirementsSize = memoryRequirements.size;

		VkMemoryAllocateInfo memAllocateInfo = {};
		{
			memAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memAllocateInfo.allocationSize = memoryRequirements.size;
			memAllocateInfo.memoryTypeIndex = m_MemoryType;

			VkResult result = vkAllocateMemory(device, &memAllocateInfo, nullptr, &m_DeviceMemory);
			assert(result == VK_SUCCESS);

#ifdef SMOLENGINE_DEBUG
			NATIVE_INFO("VulkanBuffer:: Allocating {} bytes of memory", size);
#endif // SMOLENGINE_DEBUG

		}

		void* destBuffer =  nullptr;

		VkResult map_result = vkMapMemory(device, m_DeviceMemory, 0, m_MemoryRequirementsSize, 0, &destBuffer);
		assert(map_result == VK_SUCCESS);

		memcpy(destBuffer, data, size);

		VkResult bind_result = vkBindBufferMemory(device, m_Buffer, m_DeviceMemory, offset);
		assert(bind_result == VK_SUCCESS);

		m_Size = size;
		vkUnmapMemory(device, m_DeviceMemory);
	}

	void VulkanBuffer::Create(size_t size, VkMemoryPropertyFlags memFlags, VkBufferUsageFlags usageFlags, uint32_t offset, VkSharingMode shareMode)
	{
		const auto& device = *m_Device->GetLogicalDevice();

		VkDeviceSize bufferSize = size;
		VkBufferCreateInfo bufferInfo = {};
		{
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = bufferSize;
			bufferInfo.usage = usageFlags;
			bufferInfo.sharingMode = shareMode;

			VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, &m_Buffer);
			assert(result == VK_SUCCESS);
		}

		assert(m_Buffer != VK_NULL_HANDLE);

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(device, m_Buffer, &memoryRequirements);
		m_MemoryType = FindMemoryType(memoryRequirements.memoryTypeBits, memFlags);
		m_MemoryRequirementsSize = memoryRequirements.size;

		VkMemoryAllocateInfo memAllocateInfo = {};
		{
			memAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memAllocateInfo.allocationSize = memoryRequirements.size;
			memAllocateInfo.memoryTypeIndex = m_MemoryType;

			VkResult result = vkAllocateMemory(device, &memAllocateInfo, nullptr, &m_DeviceMemory);
			assert(result == VK_SUCCESS);

#ifdef SMOLENGINE_DEBUG
			NATIVE_INFO("VulkanBuffer:: Allocating {} bytes of memory", size);
#endif // SMOLENGINE_DEBUG

		}

		VkResult bind_result = vkBindBufferMemory(device, m_Buffer, m_DeviceMemory, offset);
		assert(bind_result == VK_SUCCESS);

		m_Size = size;
	}

	void VulkanBuffer::SetData(const void* data, size_t size, uint32_t offset)
	{
		void* dest = MapMemory();
		memcpy(dest, data, size);
		UnMapMemory();
		m_Size = size;
	}

	void* VulkanBuffer::MapMemory()
	{
		uint8_t* data;
		const auto& device = *m_Device->GetLogicalDevice();
		VK_CHECK_RESULT(vkMapMemory(device, m_DeviceMemory, 0, m_MemoryRequirementsSize, 0, (void**)&data));
		return data;
	}

	void VulkanBuffer::UnMapMemory()
	{
		const auto& device = *m_Device->GetLogicalDevice();
		vkUnmapMemory(device, m_DeviceMemory);
	}

	uint32_t VulkanBuffer::GetSize() const
	{
		return m_Size;
	}

	const VkBuffer& VulkanBuffer::GetBuffer() const
	{
		return m_Buffer;
	}

	const VkDeviceMemory& VulkanBuffer::GetDeviceMemory() const
	{
		return m_DeviceMemory;
	}

	uint32_t VulkanBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags memFlags)
	{
		const auto& device_mem = *VulkanContext::GetDevice().GetMemoryProperties();

		// Iterate over all memory types available for the device used in this example
		for (uint32_t i = 0; i < device_mem.memoryHeapCount; i++)
		{
			if ((typeFilter & 1) == 1)
			{
				if ((device_mem.memoryTypes[i].propertyFlags & memFlags) == memFlags)
				{
					return i;
				}
			}

			typeFilter >>= 1;
		}

		return -1;
	}
}