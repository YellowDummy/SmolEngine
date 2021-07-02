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
	class VertexBuffer
	{
	public:

		VertexBuffer() = default;
		~VertexBuffer() = default;

		void Bind() const;
		void UnBind() const;
		void Clear();
		bool IsInitialized() const;

		// Upload
		void UploadData(const void* data, const uint32_t size, const uint32_t offset = 0);
#ifndef FROSTIUM_OPENGL_IMPL
		void CmdUpdateData(VkCommandBuffer cmdBuffer, const void* data, size_t size, uint32_t offset = 0)
		{
			m_VulkanVertexBuffer.CmdUpdateData(cmdBuffer, data, size, offset);
		}
#endif
		// Getters
		BufferLayout* GetLayout();
#ifndef FROSTIUM_OPENGL_IMPL
		VulkanVertexBuffer& GetVulkanVertexBuffer() { return m_VulkanVertexBuffer; }
#endif

		// Setters
		void SetLayout(const BufferLayout& layout);

		// Factory
		static void Create(VertexBuffer* ot_vb, uint32_t size);
		static void Create (VertexBuffer* ot_vb, void* vertices, uint32_t size, bool is_static = false);

	private:

#ifdef FROSTIUM_OPENGL_IMPL
		OpenglVertexBuffer m_OpenglVertexBuffer = {};
#else
		VulkanVertexBuffer m_VulkanVertexBuffer = {};
#endif
	};
}