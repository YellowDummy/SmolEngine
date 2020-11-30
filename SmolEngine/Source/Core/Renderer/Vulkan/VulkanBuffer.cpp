#include "stdafx.h"
#include "VulkanBuffer.h"

#include "Core/Renderer/Vulkan/VulkanContext.h"

namespace SmolEngine
{
	VulkanBuffer::VulkanBuffer()
	{

	}

	VulkanBuffer::~VulkanBuffer()
	{
		const auto& device = *VulkanContext::GetDevice().GetLogicalDevice();

		vkUnmapMemory(device, m_DeviceMemory);
		vkDestroyBuffer(device, m_Buffer, nullptr);
		vkFreeMemory(device, m_DeviceMemory, nullptr);
	}

	Ref<VulkanBuffer> VulkanBuffer::Create(const void* data, size_t size, VkMemoryPropertyFlags memFlags, VkBufferUsageFlags usageFlags, uint32_t offset, VkSharingMode shareMode)
	{
		const auto& device = *VulkanContext::GetDevice().GetLogicalDevice();

		Ref<VulkanBuffer> vkbuffer = std::make_shared<VulkanBuffer>();
		{
			VkDeviceSize bufferSize = size;
			VkBufferCreateInfo bufferInfo = {};
			{
				bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bufferInfo.size = bufferSize;
				bufferInfo.usage = usageFlags;
				bufferInfo.sharingMode = shareMode;

				VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, &vkbuffer->m_Buffer);
				assert(result == VK_SUCCESS);
			}

			VkMemoryRequirements memoryRequirements;
			vkGetBufferMemoryRequirements(device, vkbuffer->m_Buffer, &memoryRequirements);
			vkbuffer->m_MemoryType = FindMemoryType(memoryRequirements.memoryTypeBits, memFlags);

			VkMemoryAllocateInfo memAllocateInfo = {};
			{
				memAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				memAllocateInfo.allocationSize = size;
				memAllocateInfo.memoryTypeIndex = vkbuffer->m_MemoryType;

				VkResult result = vkAllocateMemory(device, &memAllocateInfo, nullptr, &vkbuffer->m_DeviceMemory);
				assert(result == VK_SUCCESS);

#ifdef SMOLENGINE_DEBUG
				NATIVE_INFO("VulkanBuffer:: Allocating {} bytes of memory", size);
#endif // SMOLENGINE_DEBUG

			}

			VkResult bind_result = vkBindBufferMemory(device, vkbuffer->m_Buffer, vkbuffer->m_DeviceMemory, offset);
			assert(bind_result == VK_SUCCESS);

			VkResult map_result = vkMapMemory(device, vkbuffer->m_DeviceMemory, offset, size, 0, &vkbuffer->mappedMemory);
			assert(map_result == VK_SUCCESS);

			//

			vkbuffer->m_Size = size;
			memcpy(vkbuffer->mappedMemory, data, size);
		}


		return vkbuffer;
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

		for (uint32_t i = 0; i < device_mem.memoryHeapCount; ++i)
		{
			if (typeFilter & (1 << i) && (device_mem.memoryTypes[i].propertyFlags & memFlags) == memFlags)
			{
				return i;
			}
		}

		return -1;
	}
}