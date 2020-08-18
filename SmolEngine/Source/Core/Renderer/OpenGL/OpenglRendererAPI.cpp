#include "stdafx.h"
#include "OpenglRendererAPI.h"

#include <glad/glad.h>
#include "Core/Renderer/Buffer.h"

namespace SmolEngine
{
	OpenglRendererAPI::OpenglRendererAPI()
	{

	}

	void OpenglRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenglRendererAPI::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
	}

	void OpenglRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenglRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count)
	{
		if (count == 0)
		{
			count = vertexArray->GetIndexBuffer()->GetCount();
		}

		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenglRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}
}