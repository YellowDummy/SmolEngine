#pragma once

#include "Core/SLog.h"

#include <vulkan/vulkan.h>

#define VK_CHECK_RESULT(f)															\
{																					\
	VkResult res = (f);																\
	if (res != VK_SUCCESS)															\
	{																				\
		NATIVE_ERROR("VkResult is {}, in {} at line {}", res, __FILE__, __LINE__);  \
		assert(res == VK_SUCCESS);													\
	}																				\
}
