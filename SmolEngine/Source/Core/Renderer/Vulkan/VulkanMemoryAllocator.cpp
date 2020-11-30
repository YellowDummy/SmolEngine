#include "stdafx.h"
#include "VulkanMemoryAllocator.h"

#include "Core/SLog.h"

namespace SmolEngine
{
	bool VulkanMemoryAllocator::Allocate(const VkDevice& device, const VkMemoryRequirements& memRequirements, VkDeviceMemory* dest, uint32_t memType)
	{
		VkMemoryAllocateInfo memAllocateInfo = {};
		{
			memAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memAllocateInfo.allocationSize = memRequirements.size;
			memAllocateInfo.memoryTypeIndex = memType;

			VkResult result = vkAllocateMemory(device, &memAllocateInfo, nullptr, dest);
			assert(result == VK_SUCCESS);

			return result == VK_SUCCESS;
		}
	}
}