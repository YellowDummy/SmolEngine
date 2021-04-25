#pragma once
#ifndef FROSTIUM_OPENGL_IMPL
#include "Common/SLog.h"
#include <vulkan/include/vulkan/vulkan.h>

#define VK_CHECK_RESULT(f)															\
{																					\
	VkResult res = (f);																\
	if (res != VK_SUCCESS)															\
	{																				\
		NATIVE_ERROR("VkResult is {}, in {} at line {}", res, __FILE__, __LINE__);  \
		assert(res == VK_SUCCESS);													\
	}																				\
}
#endif