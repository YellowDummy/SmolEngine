#include "stdafx.h"
#include "VulkanUtils.h"

namespace SmolEngine
{
    void* VulkanUtils::AlignedAlloc(size_t size, size_t alignment)
    {
		void* data = nullptr;
#if defined(_MSC_VER) || defined(__MINGW32__)
		data = _aligned_malloc(size, alignment);
#else
		int res = posix_memalign(&data, alignment, size);
		if (res != 0)
			data = nullptr;
#endif
		return data;
    }

    void VulkanUtils::AlignedFree(void* data)
    {
#if	defined(_MSC_VER) || defined(__MINGW32__)
		_aligned_free(data);
#else
		free(data);
#endif
    }
}