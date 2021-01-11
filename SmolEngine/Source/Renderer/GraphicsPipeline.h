#pragma once
#include "Core/Core.h"
#ifndef SMOLENGINE_OPENGL_IMPL
#include "Renderer/Vulkan/VulkanPipeline.h"
#include "Renderer/Vulkan/VulkanPipelineSpecification.h"
#endif

#include "Renderer/GraphicsPipelineCreateInfos.h"

namespace SmolEngine
{
	class Framebuffer;

	enum class DrawMode : uint16_t
	{
		Triangle,
		Line,
		Fan
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

		bool Reload();

		void ClearColors(const glm::vec4& clearColors = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

		// Render Pass

		void BeginRenderPass(Ref<Framebuffer> framebuffer = nullptr);

		void EndRenderPass();

		// Cmd Buffer

		void BeginCommandBuffer(bool isSwapchainTarget = false);

		void EndCommandBuffer();

		void FlushCommandBuffer();

		// Draw

		void DrawIndexed(DrawMode mode = DrawMode::Triangle, uint32_t vertexBufferIndex = 0, uint32_t indexBufferIndex = 0, uint32_t descriptorSetIndex = 0);

		void Draw(uint32_t vertextCount, DrawMode mode = DrawMode::Triangle, uint32_t vertexBufferIndex = 0, uint32_t descriptorSetIndex = 0);

		// Submit

		void BeginBufferSubmit();

		void EndBufferSubmit();

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

		// Update Resources

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

		// Helpres

		bool IsPipelineCreateInfoValid(const GraphicsPipelineCreateInfo* pipelineInfo);

	private:

#ifndef SMOLENGINE_OPENGL_IMPL

		VulkanPipeline m_VulkanPipeline = {};
		VkCommandBuffer m_CommandBuffer = nullptr;
		uint32_t m_IsSwapchainTarget = false;
#endif
		struct PipelineState
		{
			std::vector<DrawMode> PipelineDrawModes;
			BufferLayout Layout = {};
			uint32_t Stride = 0;
			uint32_t DescriptorSets = 1;

			bool IsAlphaBlendingEnabled = false;
			std::string PipelineName = "";

		} m_State;

		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		std::vector<Ref<IndexBuffer>> m_IndexBuffers;

		Ref<VertexArray> m_VextexArray;
		Ref<Shader> m_Shader = nullptr;
		Ref<Framebuffer> m_RenderpassFramebuffer = nullptr;
	};
}