#pragma once
#ifndef SMOLENGINE_OPENGL_IMPL
#include "Core/Renderer/Vulkan/VulkanPipeline.h"
#include "Core/Renderer/Vulkan/VulkanPipelineSpecification.h"
#endif

#include "Core/Core.h"

#include "Core/Renderer/Buffer.h"
#include "Core/Renderer/Texture.h"
#include "Core/Renderer/BufferLayout.h"
#include "Core/Renderer/Shader.h"
#include "Core/Renderer/ShaderTypes.h"

namespace SmolEngine
{
	class Framebuffer;

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
		size_t Size = 0;
		size_t Stride = 0;
		size_t BuffersCount = 1;
		BufferLayout* BufferLayot = nullptr;

		void* Vertices = nullptr;
		bool IsAllocateMemOnly = false;
	};

	struct IndexBufferCreateInfo
	{
		size_t IndicesCount = 0;
		size_t BuffersCount = 1;
		uint32_t* Indices = nullptr;
	};

	struct GraphicsPipelineCreateInfo
	{
		IndexBufferCreateInfo* IndexBuffer;
		VertexBufferCreateInfo* VertexBuffer;
		GraphicsPipelineShaderCreateInfo* ShaderCreateInfo;

		bool IsAlphaBlendingEnabled = false;
		uint32_t DescriptorSets = 1;
	};

	class GraphicsPipeline
	{
	public:

		bool Create(const GraphicsPipelineCreateInfo* pipelineInfo);


		void BeginRenderPass(Ref<Framebuffer> framebuffer = nullptr, const glm::vec4& clearColors = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

		void EndRenderPass();

		void ClearColors(Ref<Framebuffer>& framebuffer);

		void BeginBufferSubmit();

		void EndBufferSubmit();


		void BeginCommandBuffer();

		void EndCommandBuffer();

		void FlushCommandBuffer();


		void DrawIndexed(uint32_t vertexBufferIndex = 0, uint32_t indexBufferIndex = 0, uint32_t descriptorSetIndex = 0);

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

		void UpdateVertextBuffer(void* vertices, size_t size, uint32_t offset = 0, uint32_t bufferIndex = 0);

		void UpdateIndexBuffer(uint32_t* indices, size_t count, uint32_t bufferIndex = 0);

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
	};
}