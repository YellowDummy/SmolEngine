#include "stdafx.h"
#include "OpenglRendererAPI.h"
#include "Renderer/Renderer.h"
#include "Renderer/Camera.h"
#include "Core/SLog.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Renderer/Buffer.h"

namespace SmolEngine
{
	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         NATIVE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       NATIVE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:          NATIVE_WARN(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: NATIVE_INFO(message); return;
		}
	}

	OpenglRendererAPI::OpenglRendererAPI()
	{
	}

	OpenglRendererAPI::~OpenglRendererAPI()
	{
	}

	void OpenglRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenglRendererAPI::Init()
	{
		glEnable(GL_ALPHA_TEST);

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDisable(GL_DEPTH_TEST);


		static bool debugInit = false;

		if (!debugInit)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(OpenGLMessageCallback, nullptr);

			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);

			debugInit = true;
		}
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

	void OpenglRendererAPI::DrawTriangle(const Ref<VertexArray> vertexArray, uint32_t count, size_t vertices)
	{
		if (vertices > 0)
		{
			glDrawArrays(GL_TRIANGLES, 1, vertices);
		}
		else
		{
			uint32_t count_t = count ? count : vertexArray->GetIndexBuffer()->GetCount();
			glDrawElements(GL_TRIANGLES, count_t, GL_UNSIGNED_INT, nullptr);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	void OpenglRendererAPI::DrawLine(const Ref<VertexArray> vertexArray, uint32_t count, size_t vertices)
	{
		if (vertices > 0)
		{
			glDrawArrays(GL_LINE_STRIP, 1, vertices);
		}
		else
		{
			uint32_t count_t = count ? count : vertexArray->GetIndexBuffer()->GetCount();
			glDrawElements(GL_LINE_STRIP, count_t, GL_UNSIGNED_INT, nullptr);
		}
	}

	void OpenglRendererAPI::DrawFan(const Ref<VertexArray> vertexArray, uint32_t count, size_t vertices)
	{
		if (vertices > 0)
		{
			glDrawArrays(GL_LINE_LOOP, 1, vertices);
		}
		else
		{
			uint32_t count_t = count ? count : vertexArray->GetIndexBuffer()->GetCount();
			glDrawElements(GL_LINE_LOOP, count_t, GL_UNSIGNED_INT, nullptr);
		}
	}

	void OpenglRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

}