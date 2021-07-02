#pragma once
#include "Common/Core.h"
#include "Common/BufferLayout.h"

#ifdef FROSTIUM_OPENGL_IMPL
#include "OpenGL/OpenglBuffer.h"
#else
#include "Vulkan/VulkanIndexBuffer.h"
#endif

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	class IndexBuffer
	{
	public:

		IndexBuffer() = default;
		~IndexBuffer() = default;

		void Bind() const;
		void UnBind() const;
		void Clear();
		bool IsInitialized() const;

		// Upload
		void UploadData(uint32_t* indices, uint32_t count);
#ifndef FROSTIUM_OPENGL_IMPL
		void CmdUpdateData(VkCommandBuffer cmdBuffer, const void* data, size_t size, uint32_t offset = 0)
		{
			m_VulkanIndexBuffer.CmdUpdateData(cmdBuffer, data, size, offset);
		}
#endif
		//  Getters
		uint32_t GetCount() const;
#ifndef FROSTIUM_OPENGL_IMPL

		VulkanIndexBuffer& GetVulkanIndexBuffer() { return m_VulkanIndexBuffer; };
#endif
		// Factory
		static void Create(IndexBuffer* out_ib, uint32_t* indices, uint32_t count, bool is_static = false);

	private:

#ifdef FROSTIUM_OPENGL_IMPL
		OpenglIndexBuffer  m_OpenglIndexBuffer = {};
#else					   
		VulkanIndexBuffer  m_VulkanIndexBuffer = {};
#endif
	};
}