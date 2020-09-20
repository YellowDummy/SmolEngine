#include "stdafx.h"
#include "Renderer2D.h"
#include "Core/Core.h"
#include "Core/Renderer/Camera.h"
#include "Renderer.h"
#include "Core/Renderer/Buffer.h"
#include "Core/Renderer/Shader.h"
#include "Core/Renderer/OpenGL/OpenglShader.h"

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SmolEngineCore.h>


namespace SmolEngine
{


	struct Renderer2DStorage
	{
		Ref<VertexArray> QuadVertexArray;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;


		//---------------------------------------------------------------------------------------------------------
		Ref<VertexArray> DebugQuadVertexArray;
		Ref<VertexArray> DebugCircleVertexArray;

		Ref<Shader> DebugTextureShader;

		int outerVertexCount;

		//---------------------------------------------------------------------------------------------------------

		Ref<Shader> Light2DShader;
	};

	static Renderer2DStorage* s_Data;

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DStorage();
		s_Data->QuadVertexArray = VertexArray::Create();

		{
			float squareVertices[5 * 4] =
			{
				-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
				 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
				 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
				-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
			};

			Ref<VertexBuffer> squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
			squareVB->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" }
				});

			s_Data->QuadVertexArray->SetVertexBuffer(squareVB);

			uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
			Ref<IndexBuffer> squareIB = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
			s_Data->QuadVertexArray->SetIndexBuffer(squareIB);

			s_Data->WhiteTexture = Texture2D::Create(1, 1);
			uint32_t whiteTextureData = 0xffffffff;
			s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

			s_Data->TextureShader = RendererCommand::LoadShader("Assets/Shaders/BaseShader.glsl");
			s_Data->TextureShader->Bind();
			s_Data->TextureShader->SetUniformInt("u_Texture", 0);
		}

		//--------------------------------------------DEBUG-------------------------------------------------//

		{
			s_Data->DebugQuadVertexArray = VertexArray::Create();

			float squareVertices[12] =
			{
				-0.5f, -0.5f, 0.0f,
				 0.5f, -0.5f, 0.0f,
				 0.5f,  0.5f, 0.0f,
				-0.5f,  0.5f, 0.0f,
			};

			Ref<VertexBuffer> squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
			squareVB->SetLayout({
				{ ShaderDataType::Float3, "a_Position" }

				});

			s_Data->DebugQuadVertexArray->SetVertexBuffer(squareVB);

			uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
			Ref<IndexBuffer> squareIB = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
			s_Data->DebugQuadVertexArray->SetIndexBuffer(squareIB);

			s_Data->DebugTextureShader = RendererCommand::LoadShader("Assets/Shaders/DebugShader.glsl");
		}


		{
			s_Data->DebugCircleVertexArray = VertexArray::Create();

			const float PI = 3.14159f;

			const size_t nVertices = 100;
			float vertices[3 * nVertices];
			const float twoPi = 8.0f * atan(1.0f);
			const size_t nSides = nVertices - 2;

			for (size_t i = 1; i < nVertices; i++)
			{
				vertices[3 * i] = cos(i * twoPi / nSides);
				vertices[3 * i + 1] = sin(i * twoPi / nSides);
				vertices[3 * i + 2] = 0.0f;
			}

			Ref<VertexBuffer> squareVB = VertexBuffer::Create(vertices, sizeof(vertices));
			squareVB->SetLayout({
				{ ShaderDataType::Float3, "a_Position" }

				});

			s_Data->DebugCircleVertexArray->SetVertexBuffer(squareVB);

		}

		//--------------------------------------------LIGHT-------------------------------------------------//

		{
			s_Data->Light2DShader = RendererCommand::LoadShader("Assets/Shaders/Light2DShader.glsl");
		}


	}

	void Renderer2D::BeginScene(Ref<OrthographicCamera> camera, float ambientValue)
	{
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetUniformMat4("u_ViewProjection", camera->GetViewProjectionMatrix());
		s_Data->TextureShader->SetUniformFloat("u_AmbientValue", ambientValue);
	}

	void Renderer2D::EndScene()
	{

	}

	void Renderer2D::DrawQuadRotated(const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const glm::vec4& color)
	{
		s_Data->TextureShader->SetUniformFloat4("u_Color", color);
		s_Data->TextureShader->SetUniformFloat("u_TilingFactor", 1.0f);
		s_Data->TextureShader->SetUniformFloat2("u_TexCoord", { -1.0f, -1.0f });
		s_Data->WhiteTexture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), worldPos)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });
		s_Data->TextureShader->SetUniformMat4("u_Transform", transform);
		s_Data->QuadVertexArray->Bind();

		RendererCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawSprite(const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const Ref<Texture2D>& texture, float repeatValue, const glm::vec4& tintColor)
	{
		s_Data->TextureShader->SetUniformFloat4("u_Color", tintColor);
		s_Data->TextureShader->SetUniformFloat("u_TilingFactor", repeatValue);
		s_Data->TextureShader->SetUniformFloat2("u_TexCoord", { -1.0f, -1.0f });
		texture->Bind();

		glm::mat4 transform;

		if (rotation == 0)
		{
			transform = glm::translate(glm::mat4(1.0f), worldPos)
				* glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });
		}
		else
		{
			transform = glm::translate(glm::mat4(1.0f), worldPos)
				* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });
		}

		s_Data->TextureShader->SetUniformMat4("u_Transform", transform);

		s_Data->QuadVertexArray->Bind();
		RendererCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawQuad(const glm::vec3& worldPos, const glm::vec2& scale, const glm::vec4& color)
	{
		s_Data->TextureShader->SetUniformFloat4("u_Color", color);
		s_Data->TextureShader->SetUniformFloat("u_TilingFactor", 1.0f);
		s_Data->TextureShader->SetUniformFloat2("u_TexCoord", { -1.0f, -1.0f });
		s_Data->WhiteTexture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), worldPos)
			* glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });

		s_Data->TextureShader->SetUniformMat4("u_Transform", transform);
		s_Data->QuadVertexArray->Bind();

		RendererCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::Shutdown()
	{
		delete s_Data;
	}

	void Renderer2D::BeginDebug(Ref<OrthographicCamera> camera)
	{
		s_Data->DebugTextureShader->Bind();
		s_Data->DebugTextureShader->SetUniformMat4("u_ViewProjection", camera->GetViewProjectionMatrix());
	}

	void Renderer2D::EndDebug()
	{

	}

	void Renderer2D::DebugDraw(DebugPrimitives type, const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const glm::vec4& color)
	{
		glm::mat4 transform;

		if (rotation == 0)
		{
			transform = glm::translate(glm::mat4(1.0f), worldPos)
				* glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });
		}
		else
		{
			transform = glm::translate(glm::mat4(1.0f), worldPos)
				* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });
		}

		switch (type)
		{
		case DebugPrimitives::Quad:
		{
			s_Data->DebugTextureShader->SetUniformFloat4("u_Color", color);
			s_Data->DebugTextureShader->SetUniformMat4("u_Transform", transform);

			s_Data->DebugQuadVertexArray->Bind();

			RendererCommand::DrawLine(s_Data->DebugQuadVertexArray);
			break;
		}
		case DebugPrimitives::Circle:
		{

			s_Data->DebugTextureShader->SetUniformFloat4("u_Color", color);
			s_Data->DebugTextureShader->SetUniformMat4("u_Transform", transform);

			s_Data->DebugCircleVertexArray->Bind();

			RendererCommand::DrawFan(s_Data->DebugCircleVertexArray);
			break;
		}
		default:
			break;
		}
	}

	void Renderer2D::DrawLight2D(const glm::vec3& worldPos, const glm::vec2& scale, const glm::vec4& color, const float lightIntensity, Ref<OrthographicCamera> camera)
	{
		s_Data->Light2DShader->Bind();
		s_Data->Light2DShader->SetUniformMat4("u_ViewProjection", camera->GetViewProjectionMatrix());

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), worldPos)
			* glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });

		s_Data->Light2DShader->SetUniformMat4("u_Transform", transform);
		s_Data->Light2DShader->SetUniformFloat4("u_Color", color);
		s_Data->Light2DShader->SetUniformFloat("u_LightIntensity", lightIntensity);

		s_Data->DebugCircleVertexArray->Bind();
		RendererCommand::DrawLight();
	}

	void Renderer2D::DrawAnimation2DPreview(Ref<OrthographicCamera> camera, float ambientValue, const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const Ref<Texture2D>& texture, float repeatValue, const glm::vec4& tintColor)
	{
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetUniformMat4("u_ViewProjection", camera->GetViewProjectionMatrix());
		s_Data->TextureShader->SetUniformFloat("u_AmbientValue", ambientValue);
		s_Data->TextureShader->SetUniformFloat4("u_Color", tintColor);
		s_Data->TextureShader->SetUniformFloat("u_TilingFactor", repeatValue);
		s_Data->TextureShader->SetUniformFloat2("u_TexCoord", { -1.0f, -1.0f });
		texture->Bind();

		glm::mat4 transform;

		if (rotation == 0)
		{
			transform = glm::translate(glm::mat4(1.0f), worldPos)
				* glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });
		}
		else
		{
			transform = glm::translate(glm::mat4(1.0f), worldPos)
				* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });
		}

		s_Data->TextureShader->SetUniformMat4("u_Transform", transform);

		s_Data->QuadVertexArray->Bind();
		RendererCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawAnimation2D(const glm::vec3& worldPos, const glm::vec2& scale, const float rotation,
		const Ref<Texture2D>& texture, float repeatValue, const glm::vec4& tintColor)
	{
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetUniformFloat4("u_Color", tintColor);
		s_Data->TextureShader->SetUniformFloat("u_TilingFactor", repeatValue);
		s_Data->TextureShader->SetUniformFloat2("u_TexCoord", { -1.0f, -1.0f });
		texture->Bind();

		glm::mat4 transform;

		if (rotation == 0)
		{
			transform = glm::translate(glm::mat4(1.0f), worldPos)
				* glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });
		}
		else
		{
			transform = glm::translate(glm::mat4(1.0f), worldPos)
				* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });
		}

		s_Data->TextureShader->SetUniformMat4("u_Transform", transform);

		s_Data->QuadVertexArray->Bind();
		RendererCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

}