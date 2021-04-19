#pragma once
#ifndef FROSTIUM_OPENGL_IMPL
#include "Common/Core.h"
#include "Vulkan/Vulkan.h"

namespace Frostium
{
	class VulkanUtils
	{
	public:

		// Wrapper functions for aligned memory allocation
		// There is currently no standard for this in C++ that works across all platforms and vendors, so we abstract this

		static void* AlignedAlloc(size_t size, size_t alignment);
		static void AlignedFree(void* data);
		static void AllocBuffer(size_t);
	};
}
#endif