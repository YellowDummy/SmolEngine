#include "stdafx.h"
#include "Buffer.h"

#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/OpenGL/OpenglBuffer.h"
#include "Core/Renderer/OpenGL/OpenglVertexArray.h"
#include "Core/SLog.h"


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

		// Vulkan

		return nullptr;
#endif

		return buffer;
	}


	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		Ref<VertexBuffer> buffer = std::make_shared<VertexBuffer>();

#ifdef SMOLENGINE_OPENGL_IMPL


		buffer->m_OpenglVertexBuffer.Init(vertices, size);
#else

		// Vulkan

		return nullptr;

#endif

		return buffer;
	}


	void IndexBuffer::Bind() const
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglIndexBuffer.Bind();
#else

		// Vulkan

#endif

	}

	void IndexBuffer::UnBind() const
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglIndexBuffer.UnBind();
#else

		// Vulkan

#endif

	}

	uint32_t IndexBuffer::GetCount() const
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		return m_OpenglIndexBuffer.GetCount();
#else

		// Vulkan

#endif

	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		Ref<IndexBuffer> buffer = std::make_shared<IndexBuffer>();

#ifdef SMOLENGINE_OPENGL_IMPL


		buffer->m_OpenglIndexBuffer.Init(indices, count);
#else

		// Vulkan

		return nullptr;
#endif

		return buffer;
	}

	void VertexArray::Bind() const
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglVertexArray.Bind();
#else

		// Vulkan

#endif

	}

	void VertexArray::UnBind() const
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglVertexArray.UnBind();
#else

		// Vulkan

#endif

	}

	void VertexArray::SetVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglVertexArray.SetVertexBuffer(vertexBuffer);
#else

		// Vulkan
#endif
	}

	void VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglVertexArray.SetIndexBuffer(indexBuffer);
#else

		// Vulkan
#endif

	}

	Ref<IndexBuffer> VertexArray::GetIndexBuffer() const
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		return m_OpenglVertexArray.GetIndexBuffer();
#else

		// Vulkan
#endif

	}

	Ref<VertexArray> VertexArray::Create()
	{
		return std::make_shared<VertexArray>();
	}

	void VertexBuffer::Bind() const
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglVertexBuffer.Bind();
#else

		// Vulkan
#endif

	}

	void VertexBuffer::UnBind() const
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglVertexBuffer.UnBind();
#else

		// Vulkan
#endif

	}

	void VertexBuffer::UploadData(const void* data, const uint32_t size, const uint32_t offset) const
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglVertexBuffer.UploadData(data, size, offset);
#else

		// Vulkan
#endif

	}

	const BufferLayout& VertexBuffer::GetLayout() const
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		return m_OpenglVertexBuffer.GetLayout();
#else
		// Vulkan
#endif

	}
}