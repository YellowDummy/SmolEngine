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
#ifdef  FROSTIUM_OPENGL_IMPL
	class IndexBuffer: OpenglIndexBuffer
#else
	class IndexBuffer: VulkanIndexBuffer
#endif
	{
	public:
		void                  Bind() const;
		void                  UnBind() const;
		void                  Clear();
		bool                  IsReady() const;
		void                  UploadData(uint32_t* indices, uint32_t count);
		uint32_t              GetCount() const;
#ifndef FROSTIUM_OPENGL_IMPL
		VulkanIndexBuffer&    GetVulkanIndexBuffer() { return *dynamic_cast<VulkanIndexBuffer*>(this); };
#endif
		static void           Create(IndexBuffer* out_ib, uint32_t* indices, uint32_t count, bool is_static = false);
	};
}