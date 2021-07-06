#pragma once
#include "Common/Core.h"
#include "Common/BufferLayout.h"

#ifdef FROSTIUM_OPENGL_IMPL
#include "OpenGL/OpenglBuffer.h"
#else
#include "Vulkan/VulkanVertexBuffer.h"
#endif

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
#ifdef  FROSTIUM_OPENGL_IMPL
	class VertexBuffer: OpenglVertexBuffer
#else
	class VertexBuffer: VulkanVertexBuffer
#endif
	{
	public:
		void                  Bind() const;
		void                  UnBind() const;
		void                  Clear();
		bool                  IsReady() const;
		void                  UploadData(const void* data, const uint32_t size, const uint32_t offset = 0);
#ifndef FROSTIUM_OPENGL_IMPL
		VulkanVertexBuffer&   GetVulkanVertexBuffer() { return *dynamic_cast<VulkanVertexBuffer*>(this); }
#endif
		// Factory
		static void           Create(VertexBuffer* ot_vb, uint32_t size);
		static void           Create (VertexBuffer* ot_vb, void* vertices, uint32_t size, bool is_static = false);
	};
}