#include "stdafx.h"
#include "OpenglBuffer.h"

#include "glad/glad.h"

namespace SmolEngine
{
	//----------------------------VERTEX-BUFFER-------------------------//

	OpenglVertexBuffer::OpenglVertexBuffer()
	{
	}

	OpenglVertexBuffer::~OpenglVertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenglVertexBuffer::Init(uint32_t size)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW); // note: we use dynamic draw!
	}

	void OpenglVertexBuffer::Init(void* vertices, uint32_t size)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	void OpenglVertexBuffer::Destroy()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenglVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void OpenglVertexBuffer::UnBind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenglVertexBuffer::UploadData(const void* data, const uint32_t size, const uint32_t offset) const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);

		//glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);

		glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	}


	//----------------------------INDEX-BUFFER-------------------------//

	OpenglIndexBuffer::OpenglIndexBuffer()
	{
	}

	OpenglIndexBuffer::~OpenglIndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenglIndexBuffer::Init(uint32_t* indices, uint32_t count)
	{
		m_Count = count;

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	void OpenglIndexBuffer::UploadData(uint32_t* indices, uint32_t count) const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_DYNAMIC_DRAW);
	}

	void OpenglIndexBuffer::Destroy() const
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenglIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void OpenglIndexBuffer::UnBind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}