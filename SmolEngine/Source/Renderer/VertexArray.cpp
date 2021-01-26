#include "stdafx.h"
#include "VertexArray.h"

#include "Renderer/Renderer.h"
#include "Renderer/OpenGL/OpenglBuffer.h"
#include "Renderer/OpenGL/OpenglVertexArray.h"
#include "Core/SLog.h"


namespace SmolEngine
{
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
#endif
	}

	void VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		m_OpenglVertexArray.SetIndexBuffer(indexBuffer);
#endif

	}

	Ref<IndexBuffer> VertexArray::GetIndexBuffer() const
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		return m_OpenglVertexArray.GetIndexBuffer();
#else
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
}