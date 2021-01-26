#include "stdafx.h"
#include "VertexBuffer.h"

namespace SmolEngine
{
	void VertexBuffer::SetLayout(const BufferLayout& layout)
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglVertexBuffer.SetLayout(layout);
#else

#endif
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		Ref<VertexBuffer> buffer = std::make_shared<VertexBuffer>();

#ifdef SMOLENGINE_OPENGL_IMPL

		buffer->m_OpenglVertexBuffer.Init(size);
#else
		buffer->m_VulkanVertexBuffer.Create(size);
#endif
		return buffer;
	}


	Ref<VertexBuffer> VertexBuffer::Create(void* vertices, uint32_t size)
	{
		Ref<VertexBuffer> buffer = std::make_shared<VertexBuffer>();

#ifdef SMOLENGINE_OPENGL_IMPL

		buffer->m_OpenglVertexBuffer.Init(vertices, size);
#else
		buffer->m_VulkanVertexBuffer.Create(vertices, size);

#endif
		return buffer;
	}

	void VertexBuffer::Bind() const
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglVertexBuffer.Bind();
#endif
	}

	void VertexBuffer::UnBind() const
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglVertexBuffer.UnBind();
#endif
	}

	void VertexBuffer::Destory()
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglVertexBuffer.Destroy();
#else
		m_VulkanVertexBuffer.Destroy();
#endif
	}

	void VertexBuffer::UploadData(const void* data, const uint32_t size, const uint32_t offset)
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglVertexBuffer.UploadData(data, size, offset);
#else

		m_VulkanVertexBuffer.SetData(data, size, offset);
#endif

	}

	const BufferLayout& VertexBuffer::GetLayout() const
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		return m_OpenglVertexBuffer.GetLayout();
#else
		// Vulkan

		BufferLayout dummy;

		return dummy;
#endif

	}
}