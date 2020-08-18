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

	struct QuadVertex
	{
		glm::vec3 wordPos;
		glm::vec4 color;
		glm::vec2 textureCoord;
		float TextureIndex, ReapeValue;
	};

	struct Renderer2DStorage
	{
		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<Shader> m_TextureShader;
		Ref<Texture> m_DummyTexture;

		//max values for a given drawcall
		const uint32_t m_MaxQuads = 22000;
		const uint32_t m_MaxVertices = m_MaxQuads * 4;
		const uint32_t m_MaxIndices = m_MaxQuads * 6;
		static const uint32_t GPUMaxTextureSlots = 32;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVerterPtr = nullptr;
		QuadVertex* QuadVerterHolder = nullptr;

		std::array<Ref<Texture>, GPUMaxTextureSlots> m_TextureSlots;
		//texture at index 0 already exist (white dummy texture)
		uint32_t m_TextureSlotIndex = 1; 

		glm::vec4 QuadVertexPositions[4];

		SmolEngine::Renderer2D::RendererData2D Stats;
	};

	static Renderer2DStorage* s_Storage;

	void Renderer2D::Init()
	{
		s_Storage = new Renderer2DStorage();
		s_Storage->m_VertexArray = VertexArray::Create();

		s_Storage->m_VertexBuffer = VertexBuffer::Create(s_Storage->m_MaxVertices * sizeof(QuadVertex));

		s_Storage->m_VertexBuffer->SetLayout(
			{
				{ShaderDataType::Float3, "a_Position"},
				{ShaderDataType::Float4, "a_Color"},
				{SmolEngine::ShaderDataType::Float2, "a_TextureCoordinates"},
				{SmolEngine::ShaderDataType::Float, "a_TextureIndex"},
				{SmolEngine::ShaderDataType::Float, "a_RepeatValue"}
			});

		s_Storage->m_VertexArray->SetVertexBuffer(s_Storage->m_VertexBuffer);

		s_Storage->QuadVerterHolder = new QuadVertex[s_Storage->m_MaxVertices];

		uint32_t* quardIndices = new uint32_t[s_Storage->m_MaxIndices];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Storage->m_MaxIndices; i += 6)
		{
			quardIndices[i + 0] = offset + 0;
			quardIndices[i + 1] = offset + 1;
			quardIndices[i + 2] = offset + 2;

			quardIndices[i + 3] = offset + 2;
			quardIndices[i + 4] = offset + 3;
			quardIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> m_IndexBuffer(IndexBuffer::Create(quardIndices, s_Storage->m_MaxIndices));
		s_Storage->m_VertexArray->SetIndexBuffer(m_IndexBuffer);
		delete[] quardIndices;

		s_Storage->m_DummyTexture = Texture2D::Create(1, 1);
		uint32_t textureData = 0xffffffff;
		s_Storage->m_DummyTexture->SetData(&textureData, sizeof(uint32_t));

		int32_t samples[s_Storage->GPUMaxTextureSlots];
		for (int32_t i = 0; i < s_Storage->GPUMaxTextureSlots; i++)
		{
			samples[i] = i;
		}

		s_Storage->m_TextureShader = RendererCommand::LoadShader("Assets/Shaders/SquareShader.glsl");
		s_Storage->m_TextureShader->Bind();
		s_Storage->m_TextureShader->SetUniformIntArray("u_Textures", samples, s_Storage->GPUMaxTextureSlots);

		s_Storage->m_TextureSlots[0] = s_Storage->m_DummyTexture;

		s_Storage->QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Storage->QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Storage->QuadVertexPositions[2] = {  0.5f, 0.5f, 0.0f,  1.0f };
		s_Storage->QuadVertexPositions[3] = { -0.5f, 0.5f, 0.0f,  1.0f };
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		s_Storage->m_TextureShader->Bind();
		s_Storage->m_TextureShader->SetUniformMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		s_Storage->QuadIndexCount = 0;
		s_Storage->QuadVerterPtr = s_Storage->QuadVerterHolder;
		s_Storage->m_TextureSlotIndex = 1;
	}

	void Renderer2D::EndScene()
	{
		uint32_t bufferSize = (uint32_t)((uint8_t*)s_Storage->QuadVerterPtr - (uint8_t*)s_Storage->QuadVerterHolder);
		s_Storage->m_VertexBuffer->UploadData(s_Storage->QuadVerterHolder, bufferSize);
		Flush();
	}

	void Renderer2D::Flush()
	{
		if (s_Storage->QuadIndexCount == 0)
		{
			return;
		}

		for (uint32_t i = 0; i < s_Storage->m_TextureSlotIndex; i++)
		{
			s_Storage->m_TextureSlots[i]->Bind(i);
		}

		RendererCommand::DrawIndexed(s_Storage->m_VertexArray, s_Storage->QuadIndexCount);
		s_Storage->Stats.DrawCalls++;
	}

	void Renderer2D::DrawQuadRotated(const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const glm::vec4& color)
	{
		if (s_Storage->QuadIndexCount >= s_Storage->m_MaxIndices) { StartNewBath(); }

		const float textureIndex = 0.0f, repeatValue = 1.0f;
		constexpr float VertexsCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), worldPos)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });

		for (size_t i = 0; i < VertexsCount; i++)
		{
			s_Storage->QuadVerterPtr->wordPos = transform * s_Storage->QuadVertexPositions[i];
			s_Storage->QuadVerterPtr->textureCoord = textureCoords[i];
			s_Storage->QuadVerterPtr->color = color;
			s_Storage->QuadVerterPtr->TextureIndex = textureIndex;
			s_Storage->QuadVerterPtr->ReapeValue = repeatValue;
			s_Storage->QuadVerterPtr++;
		}

		s_Storage->QuadIndexCount += 6;
		s_Storage->Stats.QuadCount++;
	}

	void Renderer2D::DrawSprite(const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const Ref<Texture2D>& texture, float repeatValue, const glm::vec4& tintColor)
	{
		if (s_Storage->QuadIndexCount >= s_Storage->m_MaxIndices) { StartNewBath(); }

		float textureIndex = 0.0f;
		constexpr float VertexsCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		for (uint32_t i = 1; i < s_Storage->m_TextureSlotIndex; i++)
		{
			if (*s_Storage->m_TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i; break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Storage->m_TextureSlotIndex;
			s_Storage->m_TextureSlots[s_Storage->m_TextureSlotIndex] = texture;
			s_Storage->m_TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), worldPos) 
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f});

		for (size_t i = 0; i < VertexsCount; i++)
		{
			s_Storage->QuadVerterPtr->wordPos = transform * s_Storage->QuadVertexPositions[i];
			s_Storage->QuadVerterPtr->textureCoord = textureCoords[i];
			s_Storage->QuadVerterPtr->color = tintColor;
			s_Storage->QuadVerterPtr->TextureIndex = textureIndex;
			s_Storage->QuadVerterPtr->ReapeValue = repeatValue;
			s_Storage->QuadVerterPtr++;
		}

		s_Storage->QuadIndexCount += 6;
		s_Storage->Stats.QuadCount++;
	}

	void Renderer2D::DrawTexture(const glm::mat4& transform, const glm::vec4& color)
	{
		const float textureIndex = 0.0f, repeatValue = 1.0f;
		constexpr float VertexsCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		if (s_Storage->QuadIndexCount >= s_Storage->m_MaxIndices) { StartNewBath(); }

		for (size_t i = 0; i < VertexsCount; i++)
		{
			s_Storage->QuadVerterPtr->wordPos = transform * s_Storage->QuadVertexPositions[i];
			s_Storage->QuadVerterPtr->textureCoord = textureCoords[i];
			s_Storage->QuadVerterPtr->color = color;
			s_Storage->QuadVerterPtr->TextureIndex = textureIndex;
			s_Storage->QuadVerterPtr->ReapeValue = repeatValue;
			s_Storage->QuadVerterPtr++;
		}

		s_Storage->QuadIndexCount += 6;
		s_Storage->Stats.QuadCount++;
	}

	void Renderer2D::DrawTexture(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tintColor, float repeatValue)
	{
		if (s_Storage->QuadIndexCount >= s_Storage->m_MaxIndices) { StartNewBath(); }

		float textureIndex = 0.0f;
		constexpr float VertexsCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		for (uint32_t i = 1; i < s_Storage->m_TextureSlotIndex; i++)
		{
			if (*s_Storage->m_TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i; break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Storage->m_TextureSlotIndex;
			s_Storage->m_TextureSlots[s_Storage->m_TextureSlotIndex] = texture;
			s_Storage->m_TextureSlotIndex++;
		}

		for (size_t i = 0; i < VertexsCount; i++)
		{
			s_Storage->QuadVerterPtr->wordPos = transform * s_Storage->QuadVertexPositions[i];
			s_Storage->QuadVerterPtr->textureCoord = textureCoords[i];
			s_Storage->QuadVerterPtr->color = tintColor;
			s_Storage->QuadVerterPtr->TextureIndex = textureIndex;
			s_Storage->QuadVerterPtr->ReapeValue = repeatValue;
			s_Storage->QuadVerterPtr++;
		}


		s_Storage->QuadIndexCount += 6;
		s_Storage->Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec3& worldPos, const glm::vec2& scale, const glm::vec4& color)
	{
		const float textureIndex = 0.0f, repeatValue = 1.0f;
		constexpr float VertexsCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		if (s_Storage->QuadIndexCount >= s_Storage->m_MaxIndices) { StartNewBath(); }

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), worldPos) * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });
	
		for (size_t i = 0; i < VertexsCount; i++)
		{
			s_Storage->QuadVerterPtr->wordPos = transform * s_Storage->QuadVertexPositions[i];
			s_Storage->QuadVerterPtr->textureCoord = textureCoords[i];
			s_Storage->QuadVerterPtr->color = color;
			s_Storage->QuadVerterPtr->TextureIndex = textureIndex;
			s_Storage->QuadVerterPtr->ReapeValue = repeatValue;
			s_Storage->QuadVerterPtr++;
		}

		s_Storage->QuadIndexCount += 6;
		s_Storage->Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec3& worldPos, const glm::vec2& scale, const Ref<Texture2D>& texture, float repeatValue, const glm::vec4& tintColor)
	{
		if (s_Storage->QuadIndexCount >= s_Storage->m_MaxIndices) { StartNewBath(); }

		float textureIndex = 0.0f;
		constexpr float VertexsCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), worldPos) * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });

		for (uint32_t i = 1; i < s_Storage->m_TextureSlotIndex; i++)
		{
			if (*s_Storage->m_TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i; break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Storage->m_TextureSlotIndex;
			s_Storage->m_TextureSlots[s_Storage->m_TextureSlotIndex] = texture;
			s_Storage->m_TextureSlotIndex++;
		}

		for (size_t i = 0; i < VertexsCount; i++)
		{
			s_Storage->QuadVerterPtr->wordPos = transform * s_Storage->QuadVertexPositions[i];
			s_Storage->QuadVerterPtr->textureCoord = textureCoords[i];
			s_Storage->QuadVerterPtr->color = tintColor;
			s_Storage->QuadVerterPtr->TextureIndex = textureIndex;
			s_Storage->QuadVerterPtr->ReapeValue = repeatValue;
			s_Storage->QuadVerterPtr++;
		}
		

		s_Storage->QuadIndexCount += 6;
		s_Storage->Stats.QuadCount++;
	}

	void Renderer2D::DrawSpriteSheetTexture(const glm::vec3& worldPos, const glm::vec2& scale, const Ref<SubTexture2D>& subTexture, float repeatValue, const glm::vec4& tintColor)
	{
		if (s_Storage->QuadIndexCount >= s_Storage->m_MaxIndices) { StartNewBath(); }

		float textureIndex = 0.0f;
		constexpr float VertexsCount = 4;
		const glm::vec2* textureCoords = subTexture->GetTextureCoods();
		const Ref<Texture2D> texture = subTexture->GetTexture();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), worldPos) * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });

		for (uint32_t i = 1; i < s_Storage->m_TextureSlotIndex; i++)
		{
			if (*s_Storage->m_TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i; break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Storage->m_TextureSlotIndex;
			s_Storage->m_TextureSlots[s_Storage->m_TextureSlotIndex] = texture;
			s_Storage->m_TextureSlotIndex++;
		}

		for (size_t i = 0; i < VertexsCount; i++)
		{
			s_Storage->QuadVerterPtr->wordPos = transform * s_Storage->QuadVertexPositions[i];
			s_Storage->QuadVerterPtr->textureCoord = textureCoords[i];
			s_Storage->QuadVerterPtr->color = tintColor;
			s_Storage->QuadVerterPtr->TextureIndex = textureIndex;
			s_Storage->QuadVerterPtr->ReapeValue = repeatValue;
			s_Storage->QuadVerterPtr++;
		}


		s_Storage->QuadIndexCount += 6;
		s_Storage->Stats.QuadCount++;
	}


	void Renderer2D::Shutdown()
	{
		delete s_Storage;
	}

	void Renderer2D::ResetDataStats()
	{
		memset(&s_Storage->Stats, 0, sizeof(RendererData2D));
	}

	Renderer2D::RendererData2D& Renderer2D::GetData()
	{
		return s_Storage->Stats;
	}

	void Renderer2D::StartNewBath()
	{
		EndScene();
		s_Storage->QuadIndexCount = 0;
		s_Storage->QuadVerterPtr = s_Storage->QuadVerterHolder;
		s_Storage->m_TextureSlotIndex = 1;
	}

}