#pragma once
#ifndef FROSTIUM_OPENGL_IMPL
#include "Vulkan/VulkanBuffer.h"

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	class VulkanIndexBuffer : public VulkanBuffer
	{
	public:

		VulkanIndexBuffer() = default;
		~VulkanIndexBuffer() = default;

		void Create(const uint32_t* data, uint64_t count, bool is_static);
		void Create(uint64_t size);
		uint32_t GetCount() const;

	private:

		uint32_t m_ElementsCount = 0;

	};
}
#endif