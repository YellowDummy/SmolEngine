#include "stdafx.h"
#include "Renderer2D.h"
#include "Core/Core.h"
#include "Core/Renderer/Camera.h"
#include "Renderer.h"
#include "Core/Renderer/Buffer.h"
#include "Core/Renderer/Shader.h"
#include "Core/Renderer/OpenGL/OpenglShader.h"
#include "SmolEngineCore.h"

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <filesystem>

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

		Ref<Shader> FrameBufferShader;
		Ref<VertexArray> FrameBufferVertexArray;

		std::vector<Ref<Drawable>> DrawList;
	};

	static Renderer2DStorage* s_Data;

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DStorage();

		{
			s_Data->QuadVertexArray = VertexArray::Create();

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

#ifdef SMOLENGINE_EDITOR

			s_Data->TextureShader = RendererCommand::LoadShader("../SmolEngine/Assets/Shaders/BaseShader.glsl");
#else
			std::string path = "../../SmolEngine/Assets/Shaders/BaseShader.glsl";
			std::string name = "BaseShader.glsl";

			if (FindShader(path, name))
			{
				s_Data->TextureShader = RendererCommand::LoadShader(path);
			}
#endif


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

#ifdef SMOLENGINE_EDITOR

			s_Data->DebugTextureShader = RendererCommand::LoadShader("../SmolEngine/Assets/Shaders/DebugShader.glsl");
#else

			std::string path = "../SmolEngine/Assets/Shaders/DebugShader.glsl";
			std::string name = "DebugShader.glsl";

			if (FindShader(path, name))
			{
				s_Data->DebugTextureShader = RendererCommand::LoadShader(path);
			}

#endif 
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
#ifdef SMOLENGINE_EDITOR

			s_Data->Light2DShader = RendererCommand::LoadShader("../SmolEngine/Assets/Shaders/Light2DShader.glsl");
#else
			std::string path = "../SmolEngine/Assets/Shaders/Light2DShader.glsl";
			std::string name = "Light2DShader.glsl";

			if (FindShader(path, name))
			{
				s_Data->Light2DShader = RendererCommand::LoadShader(path);

			}

#endif

		}

		//--------------------------------------------FRAMEBUFFER-------------------------------------------------//

		{
			s_Data->FrameBufferVertexArray = VertexArray::Create();

			float quadVertices[] = {
				// positions   // texCoords
				-1.0f, -1.0f,  0.0f, 0.0f,
				 1.0f, -1.0f,  1.0f, 0.0f,
				 1.0f,  1.0f,  1.0f, 1.0f,
				-1.0f,  1.0f,  0.0f, 1.0f
			};



			Ref<VertexBuffer> squareVB = VertexBuffer::Create(quadVertices, sizeof(quadVertices));
			squareVB->SetLayout({
				{ ShaderDataType::Float2, "aPos" },
				{ ShaderDataType::Float2, "aTexCoords" }
		});

			s_Data->FrameBufferVertexArray->SetVertexBuffer(squareVB);

			uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
			Ref<IndexBuffer> squareIB = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
			s_Data->FrameBufferVertexArray->SetIndexBuffer(squareIB);

#ifdef SMOLENGINE_EDITOR

			s_Data->FrameBufferShader = RendererCommand::LoadShader("../SmolEngine/Assets/Shaders/Framebuffer.glsl");
#else
			std::string path = "../SmolEngine/Assets/Shaders/Framebuffer.glsl";
			std::string name = "Framebuffer.glsl";

			if (FindShader(path, name))
			{
				s_Data->FrameBufferShader = RendererCommand::LoadShader(path);

			}

#endif

		}
	}

	void Renderer2D::BeginScene(Ref<OrthographicCamera> camera, float ambientValue)
	{
		//TEMP
		s_Data->DrawList.clear();
		s_Data->DrawList.reserve(1000);

		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetUniformMat4("u_ViewProjection", camera->GetViewProjectionMatrix());
		s_Data->TextureShader->SetUniformFloat("u_AmbientValue", ambientValue);
	}

	void Renderer2D::EndScene()
	{
		int currentLayer = 0;

		for (uint32_t i = 0; i < 11; ++i)
		{
			for (const auto drawable : s_Data->DrawList)
			{
				if (currentLayer != drawable->Layer) { continue; }

				switch (drawable->Type)
				{
				case DrawableType::Sprite:
				{
					s_Data->TextureShader->SetUniformFloat4("u_Color", drawable->Color);
					s_Data->TextureShader->SetUniformFloat("u_TilingFactor", 1.0f);
					s_Data->TextureShader->SetUniformFloat2("u_TexCoord", { -1.0f, -1.0f });
					s_Data->TextureShader->SetUniformInt("u_TextMode", false);
					drawable->Texture->Bind();

					glm::mat4 transform;

					if (drawable->Rotation == 0)
					{
						transform = glm::translate(glm::mat4(1.0f), { drawable->Transform.x,  drawable->Transform.y, 1.0})
							* glm::scale(glm::mat4(1.0f), { drawable->Scale.x, drawable->Scale.y, 1.0f });
					}
					else
					{
						transform = glm::translate(glm::mat4(1.0f), { drawable->Transform.x,  drawable->Transform.y, 1.0 })
							* glm::rotate(glm::mat4(1.0f), drawable->Rotation, { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f),
								{ drawable->Scale.x, drawable->Scale.y, 1.0f });
					}

					s_Data->TextureShader->SetUniformMat4("u_Transform", transform);
					s_Data->QuadVertexArray->Bind();
					RendererCommand::DrawIndexed(s_Data->QuadVertexArray);

					break;
				}

				case DrawableType::Animation:
				{
					s_Data->TextureShader->Bind();
					s_Data->TextureShader->SetUniformFloat4("u_Color", drawable->Color);
					s_Data->TextureShader->SetUniformFloat("u_TilingFactor", 1.0f);
					s_Data->TextureShader->SetUniformFloat2("u_TexCoord", { -1.0f, -1.0f });
					s_Data->TextureShader->SetUniformInt("u_TextMode", false);
					drawable->Texture->Bind();

					glm::mat4 transform;

					if (drawable->Rotation == 0)
					{
						transform = glm::translate(glm::mat4(1.0f), { drawable->Transform.x,  drawable->Transform.y, 1.0 })
							* glm::scale(glm::mat4(1.0f), { drawable->Scale.x, drawable->Scale.y, 1.0f });
					}
					else
					{
						transform = glm::translate(glm::mat4(1.0f), { drawable->Transform.x,  drawable->Transform.y, 1.0 })
							* glm::rotate(glm::mat4(1.0f), drawable->Rotation, { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f),
								{ drawable->Scale.x, drawable->Scale.y, 1.0f });
					}

					s_Data->TextureShader->SetUniformMat4("u_Transform", transform);

					s_Data->QuadVertexArray->Bind();
					RendererCommand::DrawIndexed(s_Data->QuadVertexArray);

					break;
				}
				case DrawableType::Text:
				{
					s_Data->TextureShader->SetUniformFloat4("u_Color", drawable->Color);
					s_Data->TextureShader->SetUniformFloat("u_TilingFactor", 1.0f);
					s_Data->TextureShader->SetUniformFloat2("u_TexCoord", { -1.0f, -1.0f });
					s_Data->TextureShader->SetUniformInt("u_TextMode", true);
					drawable->Texture->Bind();

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), { drawable->Transform.x,  drawable->Transform.y, 1 })
						* glm::scale(glm::mat4(1.0f), { drawable->Scale.x, -drawable->Scale.y, 1.0f });

					s_Data->TextureShader->SetUniformMat4("u_Transform", transform);

					s_Data->QuadVertexArray->Bind();
					RendererCommand::DrawIndexed(s_Data->QuadVertexArray);

					break;
				}
				default:
					break;
				}
			}

			currentLayer++;
		}

	}

	void Renderer2D::DrawQuadRotated(const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const glm::vec4& color)
	{
		s_Data->TextureShader->SetUniformFloat4("u_Color", color);
		s_Data->TextureShader->SetUniformFloat("u_TilingFactor", 1.0f);
		s_Data->TextureShader->SetUniformFloat2("u_TexCoord", { -1.0f, -1.0f });
		s_Data->TextureShader->SetUniformInt("u_TextMode", false);
		s_Data->WhiteTexture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), worldPos)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });
		s_Data->TextureShader->SetUniformMat4("u_Transform", transform);
		s_Data->QuadVertexArray->Bind();

		RendererCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawSprite(const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const Ref<Texture2D> texture, float repeatValue, const glm::vec4& tintColor)
	{
		auto drawable = std::make_shared<Drawable>();
		drawable->AmbientValue = 1.0f;
		drawable->Color = tintColor;
		drawable->Layer = worldPos.z;
		drawable->Rotation = rotation;
		drawable->Scale = scale;
		drawable->Texture = texture;
		drawable->Transform = worldPos;
		drawable->Type = DrawableType::Sprite;

		s_Data->DrawList.push_back(drawable);
	}

	void Renderer2D::DrawUIText(const glm::vec3& pos, const glm::vec2& scale, const Ref<Texture2D> texture, const glm::vec4& tintColor)
	{
		auto drawable = std::make_shared<Drawable>();
		drawable->AmbientValue = 1.0f;
		drawable->Color = tintColor;
		drawable->Layer = 10;
		drawable->Rotation = 0.0f;
		drawable->Scale = scale;
		drawable->Texture = texture;
		drawable->Transform = pos;
		drawable->Type = DrawableType::Text;

		s_Data->DrawList.push_back(drawable);
	}

	void Renderer2D::DrawQuad(const glm::vec3& worldPos, const glm::vec2& scale, const glm::vec4& color)
	{
		s_Data->TextureShader->SetUniformFloat4("u_Color", color);
		s_Data->TextureShader->SetUniformFloat("u_TilingFactor", 1.0f);
		s_Data->TextureShader->SetUniformFloat2("u_TexCoord", { -1.0f, -1.0f });
		s_Data->TextureShader->SetUniformInt("u_TextMode", false);
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
		s_Data->TextureShader->SetUniformInt("u_TextMode", false);
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
		const Ref<Texture2D> texture, float repeatValue, const glm::vec4& tintColor)
	{
		auto drawable = std::make_shared<Drawable>();
		drawable->AmbientValue = 1.0f;
		drawable->Color = tintColor;
		drawable->Layer = worldPos.z;
		drawable->Rotation = rotation;
		drawable->Scale = scale;
		drawable->Texture = texture;
		drawable->Transform = worldPos;
		drawable->Type = DrawableType::Animation;

		s_Data->DrawList.push_back(drawable);
	}

	void Renderer2D::DrawFrameBuffer(const uint32_t colorAttachmentID)
	{
		s_Data->FrameBufferShader->Bind();
		RendererCommand::BindTexture(colorAttachmentID);
		s_Data->FrameBufferVertexArray->Bind();
		RendererCommand::DrawIndexed(s_Data->FrameBufferVertexArray);
	}

	bool Renderer2D::FindShader(std::string& filePath, const std::string& shaderName)
	{
		using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

		for (const auto& dirEntry : recursive_directory_iterator(std::string("C:/Dev/SmolEngine/")))
		{
			if (dirEntry.path().filename() == shaderName)
			{
				filePath = dirEntry.path().u8string();
				return true;
			}
		}
	}

}