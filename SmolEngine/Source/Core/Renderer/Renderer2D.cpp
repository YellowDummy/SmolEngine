#include "stdafx.h"
#include "Renderer2D.h"
#include "Core/Core.h"
#include "Renderer.h"
#include "SmolEngineCore.h"

#include "Core/Renderer/Camera.h"
#include "Core/Renderer/Buffer.h"
#include "Core/Renderer/Shader.h"
#include "Core/Renderer/OpenGL/OpenglShader.h"
#include "Core/Renderer/GraphicsPipeline.h"

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <filesystem>

namespace SmolEngine
{
	struct Renderer2DStorage
	{
		static const uint32_t Light2DBufferMaxSize = 100;
		static const uint32_t MaxQuads = 15000;
		static const uint32_t MaxVertices = MaxQuads * 6;
		static const uint32_t MaxIndices = MaxQuads * 4;
		static const uint32_t MaxLayers = 12;

		/// Graphics Pipelines

		Ref<GraphicsPipeline> MainPipeline;

		/// Vertex Array

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexArray> DebugQuadVertexArray;
		Ref<VertexArray> DebugCircleVertexArray;
		Ref<VertexArray> FrameBufferVertexArray;

		// Vertex Buffer

		Ref<VertexBuffer> QuadVertexBuffer;

		// Shader

		Ref<Shader> DebugTextureShader;
		Ref<Shader> FrameBufferShader;

		/// Texture
		
		Ref<Texture2D> WhiteTexture;

		/// Light

		Light2DBuffer LightBuffer[Light2DBufferMaxSize];

		/// Count-track

		int32_t Light2DBufferSize = 0;
		uint32_t TotalQuadCount = 0;
		uint32_t TotalQuadIndexCount = 0;

		glm::vec4 QuadVertexPositions[4];

		/// Vextex Storage

		std::array<LayerDataBuffer, MaxLayers > Layers;
		uint32_t TextureSlotIndex = 1; // index 0 reserved for white texture
		std::vector<Ref<Texture2D>> TextureSlots;

		QuadVertex* ClearBuffer = new QuadVertex[MaxVertices];
		uint32_t MaxDataSize = 0;

		/// Temp

		struct Data
		{
			glm::mat4 viewProjectionMatrix;
			float ambientValue;
			Ref<Framebuffer> targetFramebuffer;

		} SceneData;
	};

	Renderer2DStats* Renderer2D::Stats = new Renderer2DStats();
	static Renderer2DStorage* s_Data;

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DStorage();

		// Create geometry, load shaders, generate uniform maps
		CreateBatchData();

		//CreateDebugData();
		//CreateFramebufferData();

	}

	void Renderer2D::BeginScene(const glm::mat4& viewProjectionMatrix, const float ambientValue, Ref<Framebuffer> targetFramebuffer)
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		s_Data->MainPipeline->SumbitUniform<glm::mat4>("u_ViewProjection", &viewProjectionMatrix);
		s_Data->MainPipeline->SumbitUniform<float>("u_AmbientValue", &ambientValue);

#endif
		s_Data->SceneData.viewProjectionMatrix = viewProjectionMatrix;
		s_Data->SceneData.ambientValue = ambientValue;
		s_Data->SceneData.targetFramebuffer = targetFramebuffer;

		StartNewBatch();
		Stats->Reset();
	}

	void Renderer2D::EndScene()
	{
		FlushAllLayers();
	}

	void Renderer2D::StartNewBatch()
	{
		// Resetting values
#ifndef SMOLENGINE_OPENGL_IMPL
		ClearBuffers();
#endif
		s_Data->Light2DBufferSize = 0;
		s_Data->TotalQuadCount = 0;
		s_Data->TotalQuadIndexCount = 0;
		s_Data->TextureSlotIndex = 1;
		m_RenderPassIsActive = false;

		// Resetting all layers
		for (auto& layer : s_Data->Layers)
		{
			ResetLayer(layer);
		}
	}

	void Renderer2D::FlushAllLayers()
	{
		// If true there is nothing to draw
		if (s_Data->TotalQuadIndexCount == 0)
		{
			s_Data->MainPipeline->BeginCommandBuffer();
			s_Data->MainPipeline->BeginRenderPass(s_Data->SceneData.targetFramebuffer);
			s_Data->MainPipeline->EndRenderPass();
			s_Data->MainPipeline->EndCommandBuffer();
			return;
		}

		s_Data->MainPipeline->BeginCommandBuffer();
		s_Data->MainPipeline->BeginBufferSubmit();
		{
			// Setting Light2D Data
			UploadLightUniforms();

			// Binding textures
			s_Data->MainPipeline->Update2DTextures(s_Data->TextureSlots);
			s_Data->MainPipeline->SumbitUniformBuffer(0, sizeof(glm::mat4), &s_Data->SceneData.viewProjectionMatrix);

			s_Data->MainPipeline->BeginRenderPass(s_Data->SceneData.targetFramebuffer);
			{
				// Iterating over all layers
				for (auto& layer : s_Data->Layers)
				{
					// No need to render empty layer

					if (!layer.isActive)
					{
						continue;
					}

					Stats->LayersInUse++;

					// Initializing Drawcall

					DrawLayer(layer);
				}
			}
			s_Data->MainPipeline->EndRenderPass();
		}
		s_Data->MainPipeline->EndBufferSubmit();
		s_Data->MainPipeline->EndCommandBuffer();
	}

	void Renderer2D::FlushLayer(LayerDataBuffer& layer)
	{
		if (!layer.isActive || layer.IndexCount == 0)
			return;

		s_Data->MainPipeline->BeginCommandBuffer();
		s_Data->MainPipeline->BeginBufferSubmit();
		{
			// Setting Light2D Data
			UploadLightUniforms();

			// Binding textures
			s_Data->MainPipeline->Update2DTextures(s_Data->TextureSlots);
			s_Data->MainPipeline->SumbitUniformBuffer(0, sizeof(glm::mat4), &s_Data->SceneData.viewProjectionMatrix);

			s_Data->MainPipeline->BeginRenderPass(s_Data->SceneData.targetFramebuffer);
			// Initializing Drawcall
			DrawLayer(layer);

			s_Data->MainPipeline->EndRenderPass();
		}
		s_Data->MainPipeline->EndBufferSubmit();

#ifndef SMOLENGINE_OPENGL_IMPL
		s_Data->MainPipeline->EndCommandBuffer();
		s_Data->MainPipeline->UpdateVertextBuffer(s_Data->ClearBuffer, layer.ClearSize, 0, layer.LayerIndex);
#endif
		// Resetting Data
		ResetLayer(layer);

	}

	void Renderer2D::ResetLayer(LayerDataBuffer& layer)
	{
		// Returning to the beginning of the array
		layer.BasePtr = layer.Base;

		// Resetting values
		layer.isActive = false;
		layer.IndexCount = 0;
		layer.QuadCount = 0;
		layer.ClearSize = 0;
	}

	void Renderer2D::DrawLayer(LayerDataBuffer& layer)
	{
		if (!layer.isActive || layer.IndexCount == 0)
			return;

		const uint32_t dataSize = (uint32_t)((uint8_t*)layer.BasePtr - (uint8_t*)layer.Base);
#ifndef SMOLENGINE_OPENGL_IMPL
		s_Data->MainPipeline->UpdateVertextBuffer(layer.Base, dataSize, 0, layer.LayerIndex);
		layer.ClearSize = dataSize;
#else
		s_Data->MainPipeline->UpdateVertextBuffer(layer.Base, dataSize);
#endif
		s_Data->MainPipeline->DrawIndexed(layer.LayerIndex);
		Stats->DrawCalls++;
	}

	void Renderer2D::UploadLightUniforms()
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		s_Data->MainPipeline->SumbitUniform<int>("u_Ligh2DBufferSize", &s_Data->Light2DBufferSize);
#else
		s_Data->MainPipeline->SumbitPushConstant(ShaderType::Fragment, sizeof(int32_t), &s_Data->Light2DBufferSize);
#endif

#ifdef SMOLENGINE_OPENGL_IMPL
		for (uint32_t i = 0; i < s_Data->Light2DBufferSize; i++)
		{
			auto& ref = s_Data->LightBuffer[i];

			s_Data->MainPipeline->SumbitUniform<glm::vec4>("LightData[" + std::to_string(i) + "].LightColor", &ref.Color);
			s_Data->MainPipeline->SumbitUniform<glm::vec4>("LightData[" + std::to_string(i) + "].Position", &ref.Offset);
			s_Data->MainPipeline->SumbitUniform<float>("LightData[" + std::to_string(i) + "].Radius", &ref.Attributes.r);
			s_Data->MainPipeline->SumbitUniform<float>("LightData[" + std::to_string(i) + "].Intensity", &ref.Attributes.g);

			ref.Color = glm::vec4(0.0f);
			ref.Attributes = glm::vec4(0.0f);
			ref.Offset = glm::vec4(0.0f);
		}

#else
		s_Data->MainPipeline->SumbitUniformBuffer(1, sizeof(Light2DBuffer) * (s_Data->Light2DBufferSize + 1), s_Data->LightBuffer);
#endif
	}

	void Renderer2D::ClearBuffers()
	{
		for (const auto& layer : s_Data->Layers)
		{
			if (layer.ClearSize > 0)
			{
				s_Data->MainPipeline->UpdateVertextBuffer(s_Data->ClearBuffer, layer.ClearSize, 0, layer.LayerIndex);
			}
		}
	}

	void Renderer2D::SubmitSprite(const glm::vec3& worldPos, const uint32_t layerIndex, const glm::vec2& scale, const float rotation, const Ref<Texture2D>& texture,
		const float repeatValue, const glm::vec4& tintColor)
	{
		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		float textureIndex = 0.0f;

		// Getting Layer
		auto& layer = s_Data->Layers[layerIndex];
		// Render current layer if we out of the limit
		if (layer.QuadCount >= Renderer2DStorage::MaxQuads || s_Data->TextureSlotIndex >= Renderer2D::MaxTextureSlot)
		{
			FlushAllLayers();
		}

		// If the texture already exists we just need to find texture ID
		for (uint32_t i = 1; i < s_Data->TextureSlotIndex; i++)
		{
			if (*s_Data->TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		// Else set new texture ID
		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data->TextureSlotIndex;
			s_Data->TextureSlots[s_Data->TextureSlotIndex] = texture;
			s_Data->TextureSlotIndex++;

			Stats->TexturesInUse++;
		}

		// Calculating transformation matrix
		glm::mat4 transform;
		if (rotation == 0.0f)
		{
			transform = glm::translate(glm::mat4(1.0f), worldPos)
				* glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });
		}
		else
		{
			transform = glm::translate(glm::mat4(1.0f), worldPos)
				* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });
		}

		// Note: layer is now active!
		layer.isActive = true;
		layer.IndexCount += 6;
		layer.QuadCount++;

		// Updating QuadVertex Array
		for (size_t i = 0; i < quadVertexCount; i++)
		{
			layer.BasePtr->Position = transform * s_Data->QuadVertexPositions[i]; // Note: Matrix x Vertex (in this order!)
			layer.BasePtr->Color = tintColor;
			layer.BasePtr->TextureCood = textureCoords[i];
			layer.BasePtr->TextureID = textureIndex;
			layer.BasePtr->TextMode = 0;
			layer.BasePtr++;
		}

		s_Data->TotalQuadIndexCount += 6;
		s_Data->TotalQuadCount++;
		Stats->QuadCount++;
	}

	void Renderer2D::DrawUIText(const glm::vec3& pos, const glm::vec2& scale, const Ref<Texture2D> texture, const glm::vec4& tintColor)
	{
		
	}

	void Renderer2D::SubmitQuad(const glm::vec3& worldPos, const uint32_t layerIndex, const glm::vec2& scale, const float rotation, const glm::vec4& color)
	{
		// Getting Layer
		auto& layer = s_Data->Layers[layerIndex]; // worldPos.z is Z Layer

		// Render current layer if we out of the limit
		if (layer.QuadCount >= Renderer2DStorage::MaxQuads)
		{
			FlushLayer(layer);
		}

		constexpr size_t quadVertexCount = 4;
		constexpr float textureIndex = 0.0f; // default white texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		// Calculating transformation matrix
		glm::mat4 transform;

		if (rotation == 0.0f)
		{
			transform = glm::translate(glm::mat4(1.0f), worldPos)
				* glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });
		}
		else
		{
			transform = glm::translate(glm::mat4(1.0f), worldPos)
				* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });
		}

		// Note: layer is now active!
		layer.isActive = true;
		layer.IndexCount += 6;
		layer.QuadCount++;

		// Updating QuadVertex Array
		for (size_t i = 0; i < quadVertexCount; i++)
		{
			layer.BasePtr->Position = transform * s_Data->QuadVertexPositions[i]; // Note: Matrix x Vertex (in this order!)
			layer.BasePtr->Color = color;
			layer.BasePtr->TextureCood = textureCoords[i];
			layer.BasePtr->TextureID = textureIndex;
			layer.BasePtr->TextMode = 0;
			layer.BasePtr++;
		}

		s_Data->TotalQuadIndexCount += 6;
		s_Data->TotalQuadCount++;
		Stats->QuadCount++;
	}

	void Renderer2D::Shutdown()
	{
		delete s_Data;
	}

	void Renderer2D::BeginDebug(Ref<OrthographicCamera> camera)
	{
		s_Data->DebugTextureShader->Bind();
		s_Data->DebugTextureShader->SumbitUniform<glm::mat4>("u_ViewProjection", &camera->GetViewProjectionMatrix());
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
			s_Data->DebugTextureShader->SumbitUniform<glm::vec4>("u_Color", &color);
			s_Data->DebugTextureShader->SumbitUniform<glm::mat4>("u_Transform", &transform);

			s_Data->DebugQuadVertexArray->Bind();

			RendererCommand::DrawLine(s_Data->DebugQuadVertexArray);
			break;
		}
		case DebugPrimitives::Circle:
		{

			s_Data->DebugTextureShader->SumbitUniform<glm::vec4>("u_Color", &color);
			s_Data->DebugTextureShader->SumbitUniform<glm::mat4>("u_Transform", &transform);

			s_Data->DebugCircleVertexArray->Bind();

			RendererCommand::DrawFan(s_Data->DebugCircleVertexArray);
			break;
		}
		default:
			break;
		}
	}

	void Renderer2D::SubmitLight2D(const glm::vec3& offset, const float radius, const glm::vec4& color, const float lightIntensity)
	{
		if (s_Data->Light2DBufferSize < s_Data->Light2DBufferMaxSize)
		{
			s_Data->LightBuffer[s_Data->Light2DBufferSize] = Light2DBuffer(color, { offset, 0 }, radius, lightIntensity);
			s_Data->Light2DBufferSize++;
		}
	}

	void Renderer2D::DrawAnimation2DPreview(Ref<OrthographicCamera> camera, float ambientValue, const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const Ref<Texture2D>& texture, float repeatValue, const glm::vec4& tintColor)
	{

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

		return false;
	}

	void Renderer2D::CreateBatchData()
	{
		// Creating white texture
		s_Data->WhiteTexture = Texture2D::CreateWhiteTexture();

		// Creating Layers
		{
			uint32_t index = 0;
			for (auto& layer : s_Data->Layers)
			{
				layer.Base = new QuadVertex[s_Data->MaxVertices];
				layer.isActive = false;
				layer.QuadCount = 0;
				layer.LayerIndex = index;
				index++;
			}
		}

		s_Data->TextureSlotIndex = 1;
		s_Data->TextureSlots.resize(Renderer2D::MaxTextureSlot);
		s_Data->TextureSlots[0] = s_Data->WhiteTexture;

#ifdef SMOLENGINE_EDITOR

#ifdef SMOLENGINE_OPENGL_IMPL

		//s_Data->TextureShader = RendererCommand::LoadShader("../SmolEngine/Assets/Shaders/BaseShader2D_OpenGL.glsl");
#endif

#else
		std::string path = "../../SmolEngine/Assets/Shaders/BaseShader2D_OpenGL.glsl";
		std::string name = "BaseShader.glsl";

		if (FindShader(path, name))
		{
			s_Data->TextureShader = RendererCommand::LoadShader(path);
		}
#endif
		// Creating and uploading indices
		s_Data->MainPipeline = std::make_shared<GraphicsPipeline>();

		uint32_t* quadIndices = new uint32_t[s_Data->MaxIndices];
		{
			uint32_t offset = 0;
			for (uint32_t i = 0; i < s_Data->MaxIndices; i += 6)
			{
				quadIndices[i + 0] = offset + 0;
				quadIndices[i + 1] = offset + 1;
				quadIndices[i + 2] = offset + 2;

				quadIndices[i + 3] = offset + 2;
				quadIndices[i + 4] = offset + 3;
				quadIndices[i + 5] = offset + 0;

				offset += 4;
			}
		}

		BufferLayout layout(
		{
			{ ShaderDataType::Float3, "a_Position" }, // layout(location = 0)
			{ ShaderDataType::Float4, "a_Color"},
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TextMode"},
			{ ShaderDataType::Float, "a_TextureIndex"} // layout(location = 4)
		});

		VertexBufferCreateInfo vertexBufferCI = {};
		{
			vertexBufferCI.BufferLayot = &layout;
			vertexBufferCI.Size = sizeof(QuadVertex) * s_Data->MaxVertices;
			vertexBufferCI.Stride = sizeof(QuadVertex);
#ifndef SMOLENGINE_OPENGL_IMPL
			vertexBufferCI.Count = s_Data->MaxLayers;
#endif
			vertexBufferCI.IsAllocateMemOnly = true;
		}

		IndexBufferCreateInfo indexBufferCI = {};
		{
			indexBufferCI.Count = s_Data->MaxIndices;
			indexBufferCI.Indices = quadIndices;
		}

		GraphicsPipelineShaderCreateInfo shaderCI = {};
		{
#ifdef SMOLENGINE_OPENGL_IMPL
			shaderCI.UseSingleFile = true;
			shaderCI.SingleFilePath = "../SmolEngine/Assets/Shaders/BaseShader2D_OpenGL.glsl";
#else
			shaderCI.FilePaths[ShaderType::Vertex] = "../SmolEngine/Assets/Shaders/BaseShader2D_Vulkan_Vertex.glsl";
			shaderCI.FilePaths[ShaderType::Fragment] = "../SmolEngine/Assets/Shaders/BaseShader2D_Vulkan_Fragment.glsl";

#endif
		}

		GraphicsPipelineCreateInfo graphicsPipelineCI = {};
		{
			graphicsPipelineCI.IndexBuffer = &indexBufferCI;
			graphicsPipelineCI.VertexBuffer = &vertexBufferCI;
			graphicsPipelineCI.ShaderCreateInfo = &shaderCI;

			graphicsPipelineCI.IsAlphaBlendingEnabled = true;
		}

		s_Data->MainPipeline->Create(&graphicsPipelineCI);
		delete[] quadIndices;

#ifdef SMOLENGINE_OPENGL_IMPL

		// Loading samplers
		int32_t samplers[MaxTextureSlot];
		for (uint32_t i = 0; i < MaxTextureSlot; ++i)
		{
			samplers[i] = i;
		}

		s_Data->MainPipeline->SumbitUniform<int*>("u_Textures", samplers, MaxTextureSlot);

#endif // SMOLENGINE_OPENGL_IMPL

		s_Data->QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_Data->QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	void Renderer2D::CreateDebugData()
	{

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

		/// Quad

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
		}

		// Cirlce

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
	}

	void Renderer2D::CreateFramebufferData()
	{
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
		// 

		s_Data->FrameBufferVertexArray = VertexArray::Create();

		float quadVertices[] =
		{
			// positions   // texCoords
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f,
			-1.0f,  1.0f,  0.0f, 1.0f
			};

		Ref<VertexBuffer> squareVB = VertexBuffer::Create(quadVertices, sizeof(quadVertices));
		squareVB->SetLayout(
			{
				{ ShaderDataType::Float2, "aPos" },
				{ ShaderDataType::Float2, "aTexCoords" }
			});

		s_Data->FrameBufferVertexArray->SetVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Ref<IndexBuffer> squareIB = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		s_Data->FrameBufferVertexArray->SetIndexBuffer(squareIB);
	}

}