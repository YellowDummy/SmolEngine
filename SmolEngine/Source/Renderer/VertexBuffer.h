#pragma once
#include "Core/Core.h"

#include "Renderer/BufferLayout.h"

#ifdef SMOLENGINE_OPENGL_IMPL
#include "Renderer/OpenGL/OpenglBuffer.h"
#else
#include "Renderer/Vulkan/VulkanVertexBuffer.h"
#endif

namespace SmolEngine
{
	class VertexBuffer
	{
	public:

		VertexBuffer() = default;

		~VertexBuffer() = default;

		///  Binding

		void Bind() const;

		void UnBind() const;

		void Destory();

		/// Data

		void UploadData(const void* data, const uint32_t size, const uint32_t offset = 0);


#ifndef SMOLENGINE_OPENGL_IMPL

		void CmdUpdateData(VkCommandBuffer cmdBuffer, const void* data, size_t size, uint32_t offset = 0)
		{
			m_VulkanVertexBuffer.CmdUpdateData(cmdBuffer, data, size, offset);
		}

#endif
		/// Getters

		const BufferLayout& GetLayout() const;

#ifndef SMOLENGINE_OPENGL_IMPL

		VulkanVertexBuffer& GetVulkanVertexBuffer() { return m_VulkanVertexBuffer; }
#endif
		/// Setters

		void SetLayout(const BufferLayout& layout);

		/// Factory

		static Ref<VertexBuffer> Create(uint32_t size);

		static Ref<VertexBuffer> Create(void* vertices, uint32_t size);

	private:

#ifdef SMOLENGINE_OPENGL_IMPL
		OpenglVertexBuffer m_OpenglVertexBuffer = {};
#else
		VulkanVertexBuffer m_VulkanVertexBuffer = {};
#endif

	};
}