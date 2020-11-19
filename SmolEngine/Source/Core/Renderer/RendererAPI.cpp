#include "stdafx.h"
#include "RendererAPI.h"

namespace SmolEngine
{
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

	void RendererAPI::Clear()
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglAPI.Clear();
#else

#endif

	}

	void RendererAPI::Init()
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglAPI.Init();
#else

#endif

	}

	void RendererAPI::DisableDepth()
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglAPI.DisableDepth();
#else

#endif

	}

	void RendererAPI::BindTexture(uint32_t id)
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglAPI.BindTexture(id);
#else

#endif

	}

	void RendererAPI::SetClearColor(const glm::vec4& color)
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglAPI.SetClearColor(color);
#else

#endif

	}

	void RendererAPI::DrawLine(const Ref<VertexArray> vertexArray, uint32_t count)
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglAPI.DrawLine(vertexArray, count);
#else

#endif

	}

	void RendererAPI::DrawIndexed(const Ref<VertexArray> vertexArray, uint32_t count)
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglAPI.DrawIndexed(vertexArray, count);
#else

#endif

	}

	void RendererAPI::DrawFan(const Ref<VertexArray> vertexArray, uint32_t count)
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglAPI.DrawFan(vertexArray, count);
#else

#endif

	}

	void RendererAPI::DrawLight()
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglAPI.DrawLight();
#else

#endif

	}

	void RendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglAPI.SetViewport(x, y, width, height);
#else

#endif

	}
}