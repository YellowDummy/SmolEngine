#pragma once
#ifndef FROSTIUM_OPENGL_IMPL
#include "Vulkan/VulkanBuffer.h"

namespace Frostium
{
	class VulkanStagingBuffer : public VulkanBuffer
	{
	public:

		VulkanStagingBuffer() = default;
		~VulkanStagingBuffer() = default;

		void Create(const void* data, uint64_t size);
		void Create(uint64_t size);
	};
}
#endif