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
		buffer->m_VulkanVextexBuffer.Create(size);
#endif
		return buffer;
	}


	Ref<VertexBuffer> VertexBuffer::Create(void* vertices, uint32_t size)
	{
		Ref<VertexBuffer> buffer = std::make_shared<VertexBuffer>();

#ifdef SMOLENGINE_OPENGL_IMPL

		buffer->m_OpenglVertexBuffer.Init(vertices, size);
#else
		buffer->m_VulkanVextexBuffer.Create(vertices, size);

#endif
		return buffer;
	}


	void IndexBuffer::Bind() const
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglIndexBuffer.Bind();
#endif

	}

	void IndexBuffer::UnBind() const
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglIndexBuffer.UnBind();
#endif

	}

	void IndexBuffer::UploadData(uint32_t* indices, uint32_t count)
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		m_OpenglIndexBuffer.UploadData(indices, count);
#else
		m_VulkanIndexBuffer.SetData(indices, count);
#endif
	}

	void IndexBuffer::Destory()
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglIndexBuffer.Destroy();

#else
		m_VulkanIndexBuffer.Destroy();
#endif
	}

	uint32_t IndexBuffer::GetCount() const
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		return m_OpenglIndexBuffer.GetCount();
#else
		return m_VulkanIndexBuffer.GetCount();
#endif

	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		Ref<IndexBuffer> buffer = std::make_shared<IndexBuffer>();

#ifdef SMOLENGINE_OPENGL_IMPL

		buffer->m_OpenglIndexBuffer.Init(indices, count);
#else
		buffer->m_VulkanIndexBuffer.Create(indices, count);
#endif
		return buffer;
	}

	void VertexArray::Bind() const
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglVertexArray.Bind();
#endif
	}

	void VertexArray::UnBind() const
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglVertexArray.UnBind();
#endif
	}

	void VertexArray::SetVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglVertexArray.SetVertexBuffer(vertexBuffer);
#else
		
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

		return nullptr;
#endif

	}

	Ref<VertexArray> VertexArray::Create()
	{
		auto vertexArray = std::make_shared<VertexArray>();

#ifdef SMOLENGINE_OPENGL_IMPL

		vertexArray->m_OpenglVertexArray.Init();
#endif

		return vertexArray;
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
		m_VulkanVextexBuffer.Destroy();
#endif
	}

	void VertexBuffer::UploadData(const void* data, const uint32_t size, const uint32_t offset)
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglVertexBuffer.UploadData(data, size, offset);
#else
		
		
		m_VulkanVextexBuffer.UploadData(data, size, offset);
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