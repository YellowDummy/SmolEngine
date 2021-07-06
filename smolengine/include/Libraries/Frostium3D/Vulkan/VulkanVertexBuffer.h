#pragma once
#ifndef FROSTIUM_OPENGL_IMPL
#include "Vulkan/VulkanBuffer.h"

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	class VulkanVertexBuffer : public VulkanBuffer
	{
	public:
		VulkanVertexBuffer() = default;
		~VulkanVertexBuffer() = default;

		void Init(const void* data, uint64_t size, bool is_static);
		void Init(uint64_t size);
	};
}
#endif