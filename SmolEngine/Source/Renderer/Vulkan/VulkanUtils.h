#pragma once
#include "Core/Core.h"
#include "Renderer/Vulkan/Vulkan.h"

namespace SmolEngine
{
	class VulkanUtils
	{
	public:

		// Wrapper functions for aligned memory allocation
		// There is currently no standard for this in C++ that works across all platforms and vendors, so we abstract this

		static void* AlignedAlloc(size_t size, size_t alignment);

		static void AlignedFree(void* data);
	};
}