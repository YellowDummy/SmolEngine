#include "stdafx.h"
#include "OpenglRendererAPI.h"
#include "../Renderer.h"
#include "../Camera.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
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
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		glEnable(GL_ALPHA_TEST);

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void OpenglRendererAPI::DisableDepth()
	{
		glDisable(GL_DEPTH_TEST);
	}

	void OpenglRendererAPI::BindTexture(uint32_t id)
	{
		glBindTextureUnit(0, id);
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

	void OpenglRendererAPI::DrawLine(const Ref<VertexArray>& vertexArray, uint32_t count)
	{
		if (count == 0)
		{
			count = vertexArray->GetIndexBuffer()->GetCount();
		}

		glDrawElements(GL_LINE_STRIP, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenglRendererAPI::DrawFan(const Ref<VertexArray>& vertexArray, uint32_t count)
	{
		glDrawArrays(GL_LINE_LOOP, 1, 100);
	}

	void OpenglRendererAPI::DrawLight()
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		glDrawArrays(GL_TRIANGLE_FAN, 2, 100);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void OpenglRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

}