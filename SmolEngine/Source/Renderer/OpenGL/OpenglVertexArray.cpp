#include "stdafx.h"
#include "OpenglVertexArray.h"
#include "Renderer/VertexArray.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "glad/glad.h"

namespace SmolEngine
{
	static GLenum ShaderDataTypeToOpenGL(DataTypes type)
	{
		switch (type)
		{
		case DataTypes::Float:    return GL_FLOAT;
		case DataTypes::Float2:   return GL_FLOAT;
		case DataTypes::Float3:   return GL_FLOAT;
		case DataTypes::Float4:   return GL_FLOAT;
		case DataTypes::Mat3:     return GL_FLOAT;
		case DataTypes::Mat4:     return GL_FLOAT;
		case DataTypes::Int:      return GL_INT;
		case DataTypes::Int2:     return GL_INT;
		case DataTypes::Int3:     return GL_INT;
		case DataTypes::Int4:     return GL_INT;
		case DataTypes::Bool:     return GL_BOOL;

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
