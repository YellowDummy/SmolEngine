#pragma once
#include "Core/Core.h"

#include <vulkan/vulkan.h>

namespace SmolEngine 
{
	class VulkanDevice;

	class VulkanMemoryAllocator
	{
	public:

		static bool Allocate(const VkDevice& device, const VkMemoryRequirements& memRequirements, VkDeviceMemory* dest, uint32_t memType);
	};
}