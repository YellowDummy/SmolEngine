#pragma once
#include "Core/Core.h"
#ifndef SMOLENGINE_OPENGL_IMPL
#include "Renderer/Vulkan/VulkanPipeline.h"
#endif

#include "Renderer/GraphicsPipelineShaderCreateInfo.h"
#include "Renderer/VertexArray.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Texture.h"
#include "Renderer/Shared.h"
#include "Renderer/Shader.h"
#include "Renderer/GraphicsContext.h"

namespace SmolEngine
{
	class Framebuffer;
	class CubeTexture;
	class Mesh;

	enum class DrawMode : uint16_t
	{
		Triangle,
		Line,
		Fan
	};

	enum class PipelineCreateResult : uint16_t
	{
		SUCCESS,
		ERROR_INVALID_CREATE_INFO,
		ERROR_PIPELINE_NOT_INVALIDATED,
		ERROR_PIPELINE_NOT_CREATED,
		ERROR_SHADER_NOT_RELOADED
	};

	struct GraphicsPipelineCreateInfo
	{
		//TODO: Add flags

		bool                                 bDepthTestEnabled = true;

		float                                MinDepth = 0.0f;
		float                                MaxDepth = 1.0f;

		Ref<Framebuffer>                     TargetFramebuffer = nullptr;

		uint32_t                             DescriptorSets = 1;
		GraphicsPipelineShaderCreateInfo*    ShaderCreateInfo = nullptr;
		std::string                          PipelineName = "";
		std::vector<DrawMode>                PipelineDrawModes = { DrawMode::Triangle };
		std::vector<VertexInputInfo>         VertexInputInfos;
	};

	class GraphicsPipeline
	{
	public:

		~GraphicsPipeline();

		// Main

		PipelineCreateResult Create(GraphicsPipelineCreateInfo* pipelineInfo);

		PipelineCreateResult Reload();


		void Destroy();

		void ClearColors(const glm::vec4& clearColors = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

		// Render Pass

		void BeginRenderPass();

		void EndRenderPass();

		// Cmd Buffer

		void BeginCommandBuffer(bool isMainCmdBufferInUse = false);

		void EndCommandBuffer();

		void FlushCommandBuffer();

		// Draw

		void DrawIndexed(DrawMode mode = DrawMode::Triangle, uint32_t vertexBufferIndex = 0, 
			uint32_t indexBufferIndex = 0, uint32_t descriptorSetIndex = 0);

		void Draw(uint32_t vertextCount, DrawMode mode = DrawMode::Triangle,
			uint32_t vertexBufferIndex = 0, uint32_t descriptorSetIndex = 0);

		void DrawMesh(Mesh* mesh, DrawMode mode = DrawMode::Triangle,
			uint32_t instances = 1, uint32_t descriptorSetIndex = 0);

		// Submit

		void BeginBufferSubmit();

		void EndBufferSubmit();

		void SubmitBuffer(uint32_t bindingPoint, size_t size, const void* data, uint32_t offset = 0);

		template<typename T>
		void SubmitUniform(const std::string& name, const void* data, uint32_t arrayElements = 0, size_t size = 0)
		{
#ifdef SMOLENGINE_OPENGL_IMPL
			m_Shader->Bind();
			m_Shader->SumbitUniform<T>(name, data, arrayElements, size);
#endif
		}

		void SubmitPushConstant(ShaderType shaderStage, size_t size, const void* data);

		// Update Resources

		void SetVertexBuffers(std::vector<Ref<VertexBuffer>> buffer);

		void SetIndexBuffers(std::vector<Ref<IndexBuffer>> buffer);

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

		bool UpdateVulkanImageDescriptor(uint32_t bindingPoint, const VkDescriptorImageInfo& imageInfo, uint32_t descriptorSetIndex = 0)
		{
			return m_VulkanPipeline.m_Descriptors[descriptorSetIndex].UpdateImageResource(bindingPoint, imageInfo);
		}

#endif
		bool UpdateSamplers(const std::vector<Ref<Texture>>& textures, uint32_t bindingPoint, uint32_t descriptorSetIndex = 0);

		bool UpdateSampler(Ref<Texture>& tetxure, uint32_t bindingPoint, uint32_t descriptorSetIndex = 0);

		bool UpdateCubeMap(const Ref<Texture>& cubeMap, uint32_t bindingPoint, uint32_t descriptorSetIndex = 0);

#ifndef SMOLENGINE_OPENGL_IMPL

		const VkPipeline& GetVkPipeline(DrawMode mode)
		{
			return m_VulkanPipeline.GetVkPipeline(mode);
		}

		const VulkanShader* GetVulkanShader() const
		{
			return m_Shader->GetVulkanShader();
		}

		VkCommandBuffer GetVkCommandBuffer() const
		{
			return m_CommandBuffer;
		}

		void SetCommandBuffer(VkCommandBuffer cmdBuffer)
		{
			m_CommandBuffer = cmdBuffer;
		}
#endif

#ifdef SMOLENGINE_OPENGL_IMPL

		void BindOpenGLShader()
		{
			m_Shader->Bind();
		}
#endif

	private:

		// Helpres

		bool IsPipelineCreateInfoValid(const GraphicsPipelineCreateInfo* pipelineInfo);

	private:

#ifndef SMOLENGINE_OPENGL_IMPL
		VulkanPipeline                    m_VulkanPipeline = {};
		VkCommandBuffer                   m_CommandBuffer = nullptr;
		bool                              m_IsMainCmdBufferInUse = false;
#endif
		Ref<VertexArray>                  m_VextexArray = nullptr;
		Ref<Shader>                       m_Shader = nullptr;
		GraphicsContext*                  m_GraphicsContext = nullptr;
		GraphicsPipelineCreateInfo        m_PiplineCreateInfo;

		std::vector<Ref<VertexBuffer>>    m_VertexBuffers;
		std::vector<Ref<IndexBuffer>>     m_IndexBuffers;
	};
}