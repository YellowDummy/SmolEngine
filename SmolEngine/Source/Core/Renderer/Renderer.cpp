#include "stdafx.h"
#include "Renderer.h"

#include "Buffer.h"

#include "Core/Renderer/Shader.h"
#include "Core/Renderer/Camera.h"
#include "Core/Renderer/OpenGL/OpenglShader.h"
#include "Core/Renderer/OpenGL/OpenglRendererAPI.h"
#include "Core/Renderer/Renderer2D.h"

namespace SmolEngine
{
	RendererAPI* RendererCommand::s_RendererAPI = new RendererAPI();

	Renderer::SceneData* Renderer::m_SceneData = new SceneData;

	void Renderer::Init()
	{
		RendererCommand::Init();
	}

	void Renderer::BeginScene(OrthographicCamera& cameraRef)
	{
		m_SceneData->ViewProjectionMatrix = cameraRef.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform, const glm::vec4& color)
	{
		shader->Bind();
		if (color != glm::vec4(1.0f))
		{
			shader->SumbitUniform<glm::vec4>("u_Color", &color);
		}

		shader->SumbitUniform<glm::mat4>("u_Transform", &transform);
		shader->SumbitUniform<glm::mat4>("u_ViewProjection", &m_SceneData->ViewProjectionMatrix);

		vertexArray->Bind();
	}

	void Renderer::OnWidowResize(uint32_t width, uint32_t height)
	{
		RendererCommand::SetViewport(0, 0, width, height);
	}
}