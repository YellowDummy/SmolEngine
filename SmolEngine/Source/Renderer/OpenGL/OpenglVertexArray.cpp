#include "stdafx.h"
#include "OpenglVertexArray.h"
#include "Renderer/VertexArray.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "glad/glad.h"

namespace SmolEngine
{
	static GLenum ShaderDataTypeToOpenGL(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return GL_FLOAT;
		case ShaderDataType::Float2:   return GL_FLOAT;
		case ShaderDataType::Float3:   return GL_FLOAT;
		case ShaderDataType::Float4:   return GL_FLOAT;
		case ShaderDataType::Mat3:     return GL_FLOAT;
		case ShaderDataType::Mat4:     return GL_FLOAT;
		case ShaderDataType::Int:      return GL_INT;
		case ShaderDataType::Int2:     return GL_INT;
		case ShaderDataType::Int3:     return GL_INT;
		case ShaderDataType::Int4:     return GL_INT;
		case ShaderDataType::Bool:     return GL_BOOL;

		default:                     return 0; abort();
		}
	}

	OpenglVertexArray::OpenglVertexArray()
	{

	}

	OpenglVertexArray::~OpenglVertexArray()
	{
		if (m_IsInitialized)
		{
			glDeleteVertexArrays(1, &m_RendererID);
		}
	}

	void OpenglVertexArray::Init()
	{
		glCreateVertexArrays(1, &m_RendererID);

		m_IsInitialized = true;
	}

	void OpenglVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenglVertexArray::UnBind() const
	{
		glBindVertexArray(0);
	}

	void OpenglVertexArray::SetVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{						
		glBindVertexArray(m_RendererID);  
		vertexBuffer->Bind();

		uint32_t index = 0;
		for (const auto& element : vertexBuffer->GetLayout())
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, element.GetComponentCount(),
				ShaderDataTypeToOpenGL(element.type),
				element.Normalized ? GL_FALSE : GL_FALSE, vertexBuffer->GetLayout().GetStride(),
				(const void*)element.offset);

			index++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}						

	void OpenglVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}
}
