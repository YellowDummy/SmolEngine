#pragma once
#include "Core/Core.h"

#ifdef SMOLENGINE_OPENGL_IMPL

#else
#include "Core/Renderer/Vulkan/VulkanPipeline.h"
#include "Core/Renderer/Vulkan/VulkanPipelineSpecification.h"
#endif

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
		void* Vertices = nullptr;
		BufferLayout* BufferLayot = nullptr;
	};

	struct IndexBufferCreateInfo
	{
		size_t Count = 0;
		uint32_t* Indices = nullptr;
	};

	struct GraphicsPipelineCreateInfo
	{
		IndexBufferCreateInfo* IndexBuffer;
		VertexBufferCreateInfo* VertexBuffer;
		GraphicsPipelineShaderCreateInfo* ShaderCreateInfo;
	};

	class GraphicsPipeline
	{
	public:

		bool Create(const GraphicsPipelineCreateInfo* pipelineInfo);

		void BeginRenderPass(Ref<Framebuffer>& framebuffer, const glm::vec4& clearColors);

		void EndRenderPass();


		void DrawIndexed();


		void SumbitUniformBuffer(uint32_t bindingPoint, size_t size, const void* data, uint32_t offset = 0);

		template<typename T>
		void SumbitUniform(const std::string& name, const void* data, size_t size = 0, uint32_t arrayElements = 0)
		{
			m_Shader->SumbitUniform<T>(name, data, arrayElements, size);
		}

		void SumbitPushConstant(ShaderType shaderStage, size_t size, const void* data);


		void UpdateVertextBuffer(float* vertices, size_t size, uint32_t offset = 0);

		void UpdateIndexBuffer(uint32_t* indices, size_t count);

		void Update2DTextures(const std::vector<Ref<Texture2D>>& textures);

	private:

		Ref<VertexBuffer> m_VertexBuffer = nullptr;
		Ref<IndexBuffer> m_IndexBuffer = nullptr;
		Ref<VertexArray> m_VextexArray = nullptr;
		Ref<Shader> m_Shader = nullptr;

#ifndef SMOLENGINE_OPENGL_IMPL

		VulkanPipeline m_VulkanPipeline = {};
		VkCommandBuffer m_CommandBuffer;
#endif
	};
}