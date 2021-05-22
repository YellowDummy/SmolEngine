#pragma once
#ifndef FROSTIUM_OPENGL_IMPL
#include "Common/Core.h"
#include "Vulkan/Vulkan.h"

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	class VulkanDevice;

	class VulkanMemoryAllocator
	{
	public:

		static bool Allocate(VkDevice device, const VkMemoryRequirements& memRequirements,
			VkDeviceMemory* dest, uint32_t memoryTypeIndex);

		static bool Allocate(const VulkanDevice* device, const VkMemoryRequirements& memRequirements,
			VkDeviceMemory* dest, VkMemoryPropertyFlags flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	};
}
#endif