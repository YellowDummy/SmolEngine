#include "stdafx.h"
#include "Renderer2D.h"
#include "Core/Core.h"
#include "Renderer.h"
#include "SmolEngineCore.h"

#include "Renderer/VertexArray.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Shader.h"
#include "Renderer/OpenGL/OpenglShader.h"
#include "Renderer/GraphicsPipeline.h"
#include "ECS/Systems/CommandSystem.h"

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <filesystem>

namespace SmolEngine
{
	static const uint32_t s_SamplersBindingPoint = 2;

	struct Renderer2DStorage
	{
		static const uint32_t Light2DBufferMaxSize = 100;
		static const uint32_t MaxQuads = 15000;
		static const uint32_t MaxVertices = MaxQuads * 6;
		static const uint32_t MaxIndices = MaxQuads * 4;
		static const uint32_t MaxLayers = 12;

		/// Graphics Pipelines

		Ref<GraphicsPipeline> MainPipeline = nullptr;
		Ref<GraphicsPipeline> DebugPipeline = nullptr;

		/// Texture
		
		Ref<Texture> WhiteTexture = nullptr;

		/// Light

		Light2DBuffer LightBuffer[Light2DBufferMaxSize];

		/// Count-track

		int32_t Light2DBufferSize = 0;
		uint32_t TotalQuadCount = 0;
		uint32_t TotalQuadIndexCount = 0;

		glm::vec4 QuadVertexPositions[4];

		/// Vextex Storage

		std::array<LayerDataBuffer, MaxLayers > Layers;

		QuadVertex* ClearBuffer = new QuadVertex[MaxVertices];
		uint32_t MaxDataSize = 0;

		struct Data
		{
			glm::mat4 viewProjectionMatrix;
			float ambientValue;
			Ref<Framebuffer> targetFramebuffer;

		} SceneData;

		// Debug

		struct DebugPushConstant
		{
			glm::mat4 transform = glm::mat4(1.0f);
			glm::vec4 color = glm::vec4(1.0f);

		} DebugPushConstant;
	};

	Renderer2DStats* Renderer2D::Stats = new Renderer2DStats();
	static Renderer2DStorage* s_Data;

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DStorage();

		s_Data->QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_Data->QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		CreateBatchData();
		CreateDebugData();
	}

	void Renderer2D::BeginScene(const glm::mat4& viewProjectionMatrix, const float ambientValue, Ref<Framebuffer> targetFramebuffer)
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		s_Data->MainPipeline->SumbitUniform<glm::mat4>("u_ViewProjection", &viewProjectionMatrix);
		s_Data->MainPipeline->SumbitUniform<float>("u_AmbientValue", &ambientValue);
#endif

		s_Data->MainPipeline->BeginCommandBuffer();
		s_Data->MainPipeline->BeginRenderPass(targetFramebuffer);
		{
			s_Data->MainPipeline->ClearColors();
		}
		s_Data->MainPipeline->EndRenderPass();
		s_Data->MainPipeline->BeginBufferSubmit();

		s_Data->SceneData.viewProjectionMatrix = viewProjectionMatrix;
		s_Data->SceneData.ambientValue = ambientValue;
		s_Data->SceneData.targetFramebuffer = targetFramebuffer;

		StartNewBatch();
		Stats->Reset();
	}

	void Renderer2D::EndScene()
	{
		FlushAllLayers();
#ifndef SMOLENGINE_OPENGL_IMPL
		s_Data->MainPipeline->EndBufferSubmit();
		s_Data->MainPipeline->EndCommandBuffer();
#endif
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
			return;
		}

		// Setting Light2D Data
		UploadLightUniforms();

#ifndef SMOLENGINE_OPENGL_IMPL
		for (auto& layer : s_Data->Layers)
		{
			if (!layer.isActive)
			{
				continue;
			}

			s_Data->MainPipeline->UpdateSamplers(layer.TextureSlots, s_SamplersBindingPoint,  layer.LayerIndex);
		}
#endif

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

				DrawLayer(layer);
				Stats->LayersInUse++;
			}
		}
		s_Data->MainPipeline->EndRenderPass();
	}

	void Renderer2D::FlushLayer(LayerDataBuffer& layer)
	{
		if (!layer.isActive || layer.IndexCount == 0)
			return;

		// Setting Light2D Data
		UploadLightUniforms();

		// Binding textures
		s_Data->MainPipeline->UpdateSamplers(layer.TextureSlots, s_SamplersBindingPoint, layer.LayerIndex);
		s_Data->MainPipeline->SumbitUniformBuffer(0, sizeof(glm::mat4), &s_Data->SceneData.viewProjectionMatrix);

		s_Data->MainPipeline->BeginRenderPass(s_Data->SceneData.targetFramebuffer);
		{
			DrawLayer(layer);
		}
		s_Data->MainPipeline->EndRenderPass();

#ifndef SMOLENGINE_OPENGL_IMPL
		s_Data->MainPipeline->EndCommandBuffer();
		s_Data->MainPipeline->BeginCommandBuffer();

		s_Data->MainPipeline->UpdateVertextBuffer(s_Data->ClearBuffer, layer.ClearSize, layer.LayerIndex);

		m_RenderResetted = true;
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
		layer.TextureSlotIndex = 1;
		layer.TextureSlots[0] = s_Data->WhiteTexture;
	}

	void Renderer2D::DrawLayer(LayerDataBuffer& layer)
	{
		if (!layer.isActive || layer.IndexCount == 0)
			return;

		const uint32_t dataSize = (uint32_t)((uint8_t*)layer.BasePtr - (uint8_t*)layer.Base);
#ifndef SMOLENGINE_OPENGL_IMPL
		s_Data->MainPipeline->UpdateVertextBuffer(layer.Base, dataSize, layer.LayerIndex);
		layer.ClearSize = dataSize;

		struct PushConstant
		{
			glm::mat4 cameraViewProj;
			float LightCount;
		} ps;

		ps.cameraViewProj = s_Data->SceneData.viewProjectionMatrix;
		ps.LightCount = static_cast<float>(s_Data->Light2DBufferSize);

		s_Data->MainPipeline->SumbitPushConstant(ShaderType::Vertex, sizeof(PushConstant), &ps);
		s_Data->MainPipeline->DrawIndexed(DrawMode::Triangle, layer.LayerIndex, 0, layer.LayerIndex);
#else
		// Binding textures
		s_Data->MainPipeline->UpdateSamplers(layer.TextureSlots, layer.LayerIndex);
		s_Data->MainPipeline->UpdateVertextBuffer(layer.Base, dataSize);
		s_Data->MainPipeline->DrawIndexed();
#endif
		Stats->DrawCalls++;
	}

	void Renderer2D::UploadLightUniforms()
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		s_Data->MainPipeline->SumbitUniform<int>("u_Ligh2DBufferSize", &s_Data->Light2DBufferSize);
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

	void Renderer2D::SubmitSprite(const glm::vec3& worldPos, const uint32_t layerIndex,
		const glm::vec2& scale, const float rotation, const Ref<Texture>& texture,
		const float repeatValue, const glm::vec4& tintColor)
	{
		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		float textureIndex = 0.0f;

		// Getting Layer
		auto& layer = s_Data->Layers[layerIndex];
		// Render current layer if we out of the limit
		if (layer.QuadCount >= (Renderer2DStorage::MaxQuads - 1) || layer.TextureSlotIndex >= Renderer2D::MaxTextureSlot)
		{
			FlushLayer(layer);
		}

		// If the texture already exists we just need to find texture ID
		for (uint32_t i = 1; i < layer.TextureSlotIndex; i++)
		{
			if (*layer.TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		// Else set new texture ID
		if (textureIndex == 0.0f)
		{
			textureIndex = (float)layer.TextureSlotIndex;
			layer.TextureSlots[layer.TextureSlotIndex] = texture;
			layer.TextureSlotIndex++;

			Stats->TexturesInUse++;
		}

		// Calculating transformation matrix
		glm::mat4 transform;
		CommandSystem::ComposeTransform(worldPos, { rotation, 0, 0 }, { scale, 0 }, false, transform);

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

	void Renderer2D::DrawUIText(const glm::vec3& pos, const glm::vec2& scale,
		const Ref<Texture> texture, const glm::vec4& tintColor)
	{
		
	}

	void Renderer2D::SubmitQuad(const glm::vec3& worldPos, const uint32_t layerIndex, const glm::vec2& scale, const float rotation, const glm::vec4& color)
	{
		// Getting Layer
		auto& layer = s_Data->Layers[layerIndex]; // worldPos.z is Z Layer

		// Render current layer if we out of the limit
		if (layer.QuadCount >= (Renderer2DStorage::MaxQuads - 1))
		{
			FlushLayer(layer);
		}

		constexpr size_t quadVertexCount = 4;
		constexpr float textureIndex = 0.0f; // default white texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		// Calculating transformation matrix
		glm::mat4 transform;
		CommandSystem::ComposeTransform(worldPos, { rotation, 0, 0 }, { scale, 0 }, false, transform);

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

	void Renderer2D::BeginDebug()
	{
		s_Data->DebugPipeline->BeginCommandBuffer();
		s_Data->DebugPipeline->BeginBufferSubmit();
		s_Data->DebugPipeline->BeginRenderPass(s_Data->SceneData.targetFramebuffer);
	}

	void Renderer2D::EndDebug()
	{
		s_Data->DebugPipeline->EndRenderPass();
		s_Data->DebugPipeline->EndBufferSubmit();
		s_Data->DebugPipeline->EndCommandBuffer();
	}

	void Renderer2D::DebugDrawQuad(const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const glm::vec4& color)
	{
		glm::mat4 transform;
		CommandSystem::ComposeTransform(worldPos, { rotation, 0, 0 }, { scale, 0 }, false, transform);
		s_Data->DebugPushConstant.transform = s_Data->SceneData.viewProjectionMatrix * transform;
		s_Data->DebugPushConstant.color = color;

#ifdef SMOLENGINE_OPENGL_IMPL
		s_Data->DebugPipeline->SumbitUniform<glm::mat4>("u_Transform", &s_Data->DebugPushConstant.transform);
		s_Data->DebugPipeline->SumbitUniform<glm::vec4>("u_Color", &s_Data->DebugPushConstant.color);
#else
		s_Data->DebugPipeline->SumbitPushConstant(ShaderType::Vertex, sizeof(s_Data->DebugPushConstant), &s_Data->DebugPushConstant);
#endif
		s_Data->DebugPipeline->DrawIndexed(DrawMode::Line);
	}

	void Renderer2D::DebugDrawCircle(const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const glm::vec4& color)
	{
		glm::mat4 transform;
		CommandSystem::ComposeTransform(worldPos, { rotation, 0, 0 }, { scale, 0 }, false, transform);
		s_Data->DebugPushConstant.transform = s_Data->SceneData.viewProjectionMatrix * transform;
		s_Data->DebugPushConstant.color = color;

#ifdef SMOLENGINE_OPENGL_IMPL
		s_Data->DebugPipeline->SumbitUniform<glm::mat4>("u_Transform", &s_Data->DebugPushConstant.transform);
		s_Data->DebugPipeline->SumbitUniform<glm::vec4>("u_Color", &s_Data->DebugPushConstant.color);
#else
		s_Data->DebugPipeline->SumbitPushConstant(ShaderType::Vertex, sizeof(s_Data->DebugPushConstant), &s_Data->DebugPushConstant);
#endif
		s_Data->DebugPipeline->Draw(3000, DrawMode::Fan, 1);
	}

	void Renderer2D::DebugDrawLine(const glm::vec3& startPos, const glm::vec3& endPos, const glm::vec4& color)
	{
		glm::vec4 pos(0.0f, 0.0f, 0.0f, 1.0f);
		const uint32_t bufferIndex = 2;
		glm::mat4 start_transform, end_transform;
		CommandSystem::ComposeTransform(startPos, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, false, start_transform);
		CommandSystem::ComposeTransform(endPos, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, false, end_transform);

		s_Data->DebugPushConstant.color = color;
		s_Data->DebugPushConstant.transform = s_Data->SceneData.viewProjectionMatrix;

		DebugVertex LineVertex[2];
		LineVertex[0].Position = start_transform * pos;
		LineVertex[1].Position = end_transform * pos;

#ifdef SMOLENGINE_OPENGL_IMPL
		s_Data->DebugPipeline->SumbitUniform<glm::mat4>("u_Transform", &s_Data->DebugPushConstant.transform);
		s_Data->DebugPipeline->SumbitUniform<glm::vec4>("u_Color", &s_Data->DebugPushConstant.color);
		s_Data->DebugPipeline->UpdateVertextBuffer(&LineVertex, sizeof(LineVertex), bufferIndex);
#else
		s_Data->DebugPipeline->EndRenderPass();
		{
			s_Data->DebugPipeline->CmdUpdateVertextBuffer(&LineVertex, sizeof(LineVertex), bufferIndex);
		}
		s_Data->DebugPipeline->BeginRenderPass(s_Data->SceneData.targetFramebuffer);
		s_Data->DebugPipeline->SumbitPushConstant(ShaderType::Vertex, sizeof(s_Data->DebugPushConstant), &s_Data->DebugPushConstant);

#endif
		s_Data->DebugPipeline->Draw(2, DrawMode::Line, bufferIndex);
	}

	void Renderer2D::SubmitLight2D(const glm::vec3& offset, const float radius, const glm::vec4& color, const float lightIntensity)
	{
		if (s_Data->Light2DBufferSize < s_Data->Light2DBufferMaxSize)
		{
			s_Data->LightBuffer[s_Data->Light2DBufferSize] = Light2DBuffer(color, { offset, 0 }, radius, lightIntensity);
			s_Data->Light2DBufferSize++;
		}
	}

	void Renderer2D::DrawFrameBuffer(const uint32_t colorAttachmentID)
	{

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
		s_Data->WhiteTexture = Texture::CreateWhiteTexture();

		// Creating Layers
		{
			uint32_t index = 0;
			for (auto& layer : s_Data->Layers)
			{
				layer.Base = new QuadVertex[s_Data->MaxVertices];
				layer.TextureSlots.resize(Renderer2D::MaxTextureSlot);
				layer.TextureSlots[0] = s_Data->WhiteTexture;
				layer.TextureSlotIndex = 1;
				layer.isActive = false;
				layer.QuadCount = 0;
				layer.LayerIndex = index;
				index++;
			}
		}

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

		//VertexBuffers
		std::vector<Ref<VertexBuffer>> vertexBuffers;
		uint32_t vertexBufferCount = 1;
#ifndef SMOLENGINE_OPENGL_IMPL
		vertexBufferCount = s_Data->MaxLayers;
#endif
		vertexBuffers.resize(vertexBufferCount);
		for (uint32_t i = 0; i < vertexBufferCount; ++i)
		{
			vertexBuffers[i] = VertexBuffer::Create(sizeof(QuadVertex) * s_Data->MaxVertices);
		}

		//IndexBuffers
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(quadIndices, s_Data->MaxIndices);

		GraphicsPipelineShaderCreateInfo shaderCI = {};
		{
#ifdef SMOLENGINE_OPENGL_IMPL
			shaderCI.UseSingleFile = true;
			shaderCI.SingleFilePath = "../Resources/Shaders/BaseShader2D_OpenGL.glsl";
#else
			shaderCI.FilePaths[ShaderType::Vertex] = "../Resources/Shaders/BaseShader2D_Vulkan_Vertex.glsl";
			shaderCI.FilePaths[ShaderType::Fragment] = "../Resources/Shaders/BaseShader2D_Vulkan_Fragment.glsl";
#endif
		}

		GraphicsPipelineCreateInfo graphicsPipelineCI = {};
		{
			graphicsPipelineCI.Stride = sizeof(QuadVertex);
			graphicsPipelineCI.VertexInputLayots = { layout };
			graphicsPipelineCI.ShaderCreateInfo = &shaderCI;

			graphicsPipelineCI.IsAlphaBlendingEnabled = true;
			graphicsPipelineCI.DescriptorSets = Renderer2DStorage::MaxLayers;
			graphicsPipelineCI.PipelineName = "Renderer2D_Main";
		}

		s_Data->MainPipeline->Create(&graphicsPipelineCI);

		//
		s_Data->MainPipeline->SetVertexBuffers(vertexBuffers);
		s_Data->MainPipeline->SetIndexBuffers({ indexBuffer });

		delete[] quadIndices;

#ifdef SMOLENGINE_OPENGL_IMPL
		// Loading samplers
		int32_t samplers[MaxTextureSlot];
		for (uint32_t i = 0; i < MaxTextureSlot; ++i)
		{
			samplers[i] = i;
		}

		s_Data->MainPipeline->SumbitUniform<int*>("u_Textures", samplers, MaxTextureSlot);
		for (uint32_t i = 0; i < MaxTextureSlot; ++i)
		{
			s_Data->WhiteTexture->Bind(i);
		}


#endif // SMOLENGINE_OPENGL_IMPL
	}

	void Renderer2D::CreateDebugData()
	{
		s_Data->DebugPipeline = std::make_shared<GraphicsPipeline>();
		BufferLayout layout(
		{
			{ ShaderDataType::Float3, "a_Position" }
		});

		/// Quad

		DebugVertex QuadVertex[4];
		QuadVertex[0].Position = { -0.5f, -0.5f, 0.0f};
		QuadVertex[1].Position = { 0.5f, -0.5f, 0.0f};
		QuadVertex[2].Position = { 0.5f,  0.5f, 0.0f};
		QuadVertex[3].Position = { -0.5f,  0.5f, 0.0f};

		uint32_t quadIndices[6] = { 0, 1, 2,  2, 3, 0 };

		// Cirlce
		const size_t nVertices = 3000;
		DebugVertex* CircleVertex = new DebugVertex[nVertices];
		for (size_t i = 1; i < nVertices; i++)
		{
			CircleVertex[i].Position = glm::vec3(cos(2 * 3.14159 * i / 1000.0), sin(2 * 3.14159 * i / 1000.0), 0);
		}

		// Line
		DebugVertex LineVertex[2];
		LineVertex[0].Position = { 0.0f, 0.0f, 0.0f };
		LineVertex[1].Position = { 0.0f, 0.0f, 0.0f };

		// VertexBuffers
		Ref<VertexBuffer> QuadBuffer = VertexBuffer::Create(QuadVertex, sizeof(QuadVertex));
		Ref<VertexBuffer> CircleBuffer = VertexBuffer::Create(CircleVertex, sizeof(DebugVertex) * nVertices);
		Ref<VertexBuffer> LineBuffer = VertexBuffer::Create(LineVertex, sizeof(LineVertex));

		//IndexBuffers
		Ref<IndexBuffer> QuadIndex = IndexBuffer::Create(quadIndices, 6);

		GraphicsPipelineShaderCreateInfo shaderCI = {};
		{
#ifdef SMOLENGINE_OPENGL_IMPL
			shaderCI.UseSingleFile = true;
			shaderCI.SingleFilePath = "../Resources/Shaders/DebugShader.glsl";
#else
			shaderCI.FilePaths[ShaderType::Vertex] = "../Resources/Shaders/DebugShader_Vulkan_Vertex.glsl";
			shaderCI.FilePaths[ShaderType::Fragment] = "../Resources/Shaders/DebugShader_Vulkan_Fragment.glsl";
#endif
		}

		GraphicsPipelineCreateInfo graphicsPipelineCI = {};
		{
			graphicsPipelineCI.Stride = sizeof(DebugVertex);
			graphicsPipelineCI.VertexInputLayots = { layout };
			graphicsPipelineCI.ShaderCreateInfo = &shaderCI;
			graphicsPipelineCI.PipelineDrawModes = { DrawMode::Line, DrawMode::Fan };
			graphicsPipelineCI.PipelineName = "Renderer2D_Debug";
		}

		assert(s_Data->DebugPipeline->Create(&graphicsPipelineCI) == true);

		//
		s_Data->DebugPipeline->SetVertexBuffers({ QuadBuffer, CircleBuffer, LineBuffer });
		s_Data->DebugPipeline->SetIndexBuffers({ QuadIndex });

		delete[] CircleVertex;
	}

	void Renderer2D::CreateFramebufferData()
	{
#ifdef SMOLENGINE_EDITOR


#endif
	}

}