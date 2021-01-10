#pragma once
#ifndef SMOLENGINE_OPENGL_IMPL
#include "Renderer/Vulkan/VulkanPipeline.h"
#include "Renderer/Vulkan/VulkanPipelineSpecification.h"
#endif

#include "Core/Core.h"

#include "Renderer/Buffer.h"
#include "Renderer/Texture.h"
#include "Renderer/BufferLayout.h"
#include "Renderer/Shader.h"
#include "Renderer/ShaderTypes.h"

namespace SmolEngine
{
	class Framebuffer;

	enum class DrawMode : uint16_t
	{
		Triangle,
		Line,
		Fan
	};

	struct GraphicsPipelineShaderCreateInfo
	{
		std::unordered_map<ShaderType, std::string> FilePaths;
		std::vector<Ref<Texture2D>> Textures;
		std::string SingleFilePath = "";

		bool Optimize = false;
		bool UseSingleFile = false;
	};

	struct VertexBufferCreateInfo
	{
		size_t Stride = 0;
		size_t BuffersCount = 1;
		bool IsAllocateMemOnly = false;
		BufferLayout* BufferLayot = nullptr;

		std::vector<size_t> Sizes;
		std::vector<void*> Vertices;
	};

	struct IndexBufferCreateInfo
	{
		size_t BuffersCount = 1;

		std::vector<size_t> Sizes;
		std::vector<size_t> IndicesCounts;
		std::vector<uint32_t*> Indices;
	};

	struct GraphicsPipelineCreateInfo
	{
		IndexBufferCreateInfo* IndexBuffer;
		VertexBufferCreateInfo* VertexBuffer;
		GraphicsPipelineShaderCreateInfo* ShaderCreateInfo;

		std::vector<DrawMode> PipelineDrawModes = { DrawMode::Triangle };
		std::string PipelineName = "";
		uint32_t DescriptorSets = 1;

		bool IsAlphaBlendingEnabled = false;
	};

	class GraphicsPipeline
	{
	public:

		~GraphicsPipeline();

		bool Create(const GraphicsPipelineCreateInfo* pipelineInfo);


		void BeginRenderPass(Ref<Framebuffer> framebuffer = nullptr);

		void EndRenderPass();

		void ClearColors(Ref<Framebuffer>& framebuffer, const glm::vec4& clearColors = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

		void BeginBufferSubmit();

		void EndBufferSubmit();


		void BeginCommandBuffer();

		void EndCommandBuffer();

		void FlushCommandBuffer();


		void DrawIndexed(DrawMode mode = DrawMode::Triangle, uint32_t vertexBufferIndex = 0, uint32_t indexBufferIndex = 0, uint32_t descriptorSetIndex = 0);

		void Draw(uint32_t vertextCount, DrawMode mode = DrawMode::Triangle, uint32_t vertexBufferIndex = 0, uint32_t descriptorSetIndex = 0);


		void SumbitUniformBuffer(uint32_t bindingPoint, size_t size, const void* data, uint32_t offset = 0);

		template<typename T>
		void SumbitUniform(const std::string& name, const void* data, uint32_t arrayElements = 0, size_t size = 0)
		{
#ifdef SMOLENGINE_OPENGL_IMPL
			m_Shader->Bind();
			m_Shader->SumbitUniform<T>(name, data, arrayElements, size);
#endif
		}

		void SumbitPushConstant(ShaderType shaderStage, size_t size, const void* data);


		void UpdateVertextBuffer(void* vertices, size_t size, uint32_t bufferIndex = 0, uint32_t offset = 0);

		void UpdateIndexBuffer(uint32_t* indices, size_t count, uint32_t bufferIndex = 0, uint32_t offset = 0);

#ifndef SMOLENGINE_OPENGL_IMPL

		void CmdUpdateVertextBuffer(const void* data, size_t size, uint32_t bufferIndex = 0, uint32_t offset = 0)
		{
			m_VertexBuffers[bufferIndex]->CmdUpdateData(m_CommandBuffer, data, size, offset);
		}

		void CmdUpdateIndexBuffer(uint32_t* indices, size_t count, uint32_t bufferIndex = 0, uint32_t offset = 0)
		{
			m_IndexBuffers[bufferIndex]->CmdUpdateData(m_CommandBuffer, indices, sizeof(uint32_t) * count, offset);
		}

#endif

		void Update2DTextures(const std::vector<Ref<Texture2D>>& textures, uint32_t descriptorSetIndex = 0);

	private:

		bool IsPipelineCreateInfoValid(const GraphicsPipelineCreateInfo* pipelineInfo);

	private:

#ifndef SMOLENGINE_OPENGL_IMPL

		VulkanPipeline m_VulkanPipeline = {};
		VkCommandBuffer m_CommandBuffer = nullptr;
#endif

		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		std::vector<Ref<IndexBuffer>> m_IndexBuffers;

		Ref<VertexArray> m_VextexArray;
		Ref<Shader> m_Shader = nullptr;
		Ref<Framebuffer> m_RenderpassFramebuffer = nullptr;
	};
}