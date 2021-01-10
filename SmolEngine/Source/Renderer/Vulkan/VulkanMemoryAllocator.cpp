#include "stdafx.h"
#include "VulkanMemoryAllocator.h"

#include "Renderer/Vulkan/VulkanDevice.h"

#include "Core/SLog.h"

namespace SmolEngine
{
	bool VulkanMemoryAllocator::Allocate(VkDevice device, const VkMemoryRequirements& memRequirements, VkDeviceMemory* dest, uint32_t memoryTypeIndex)
	{
		VkMemoryAllocateInfo memAllocateInfo = {};
		{
			memAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memAllocateInfo.allocationSize = memRequirements.size;
			memAllocateInfo.memoryTypeIndex = memoryTypeIndex;

			VkResult result = vkAllocateMemory(device, &memAllocateInfo, nullptr, dest);
			assert(result == VK_SUCCESS);

			return result == VK_SUCCESS;
		}
	}

	bool VulkanMemoryAllocator::Allocate(const VulkanDevice* device, const VkMemoryRequirements& memRequirements, VkDeviceMemory* dest, VkMemoryPropertyFlags flags)
	{
		VkMemoryAllocateInfo memAllocateInfo = {};
		{
			memAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memAllocateInfo.allocationSize = memRequirements.size;
			memAllocateInfo.memoryTypeIndex = device->GetMemoryTypeIndex(memRequirements.memoryTypeBits, flags);

			VkResult result = vkAllocateMemory(device->GetLogicalDevice(), &memAllocateInfo, nullptr, dest);
			assert(result == VK_SUCCESS);

			NATIVE_INFO("VulkanMemoryAllocator allocated {} bytes of memory", memRequirements.size);
			return result == VK_SUCCESS;
		}
	}
}