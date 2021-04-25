#pragma once
#include "Common/Core.h"
#include "GraphicsContext.h"
#ifndef FROSTIUM_OPENGL_IMPL
#include "Vulkan/VulkanPipeline.h"
#endif

#include "Common/GraphicsPipelineInfos.h"
#include "Common/VertexArray.h"
#include "Common/VertexBuffer.h"
#include "Common/IndexBuffer.h"
#include "Common/Texture.h"
#include "Common/Common.h"
#include "Common/Shader.h"

namespace Frostium
{
	class Framebuffer;
	class CubeTexture;
	class Mesh;

	class GraphicsPipeline
	{
	public:

		GraphicsPipeline() = default;
		~GraphicsPipeline();

		PipelineCreateResult Create(GraphicsPipelineCreateInfo* pipelineInfo);
		PipelineCreateResult Reload();
		void Destroy();

		// Render Pass
		void ClearColors(const glm::vec4& clearColors = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		void BeginRenderPass(uint32_t framebufferIndex = 0, bool flip = false);
		void EndRenderPass();

		// Cmd Buffer
		void BeginCommandBuffer(bool isMainCmdBufferInUse = false);
		void EndCommandBuffer();

		// Draw
		void DrawIndexed(DrawMode mode = DrawMode::Triangle, uint32_t vertexBufferIndex = 0,  uint32_t indexBufferIndex = 0, uint32_t descriptorSetIndex = 0);
		void DrawIndexed(VertexBuffer* vb, IndexBuffer* ib, DrawMode mode = DrawMode::Triangle, uint32_t descriptorSetIndex = 0);
		void Draw(VertexBuffer* vb, uint32_t vertextCount, DrawMode mode = DrawMode::Triangle, uint32_t descriptorSetIndex = 0);
		void Draw(uint32_t vertextCount, DrawMode mode = DrawMode::Triangle, uint32_t vertexBufferIndex = 0, uint32_t descriptorSetIndex = 0);
		void DrawMesh(Mesh* mesh, uint32_t instances = 1, DrawMode mode = DrawMode::Triangle, uint32_t descriptorSetIndex = 0);

		// Resources
		void SubmitBuffer(uint32_t bindingPoint, size_t size, const void* data, uint32_t offset = 0);
		void SubmitPushConstant(ShaderType shaderStage, size_t size, const void* data);
		void SetVertexBuffers(const std::vector<Ref<VertexBuffer>>& buffer);
		void SetIndexBuffers(const std::vector<Ref<IndexBuffer>>& buffer);
		void UpdateVertextBuffer(void* vertices, size_t size, uint32_t bufferIndex = 0, uint32_t offset = 0);
		void UpdateIndexBuffer(uint32_t* indices, size_t count, uint32_t bufferIndex = 0, uint32_t offset = 0);
#ifndef FROSTIUM_OPENGL_IMPL
		void CmdUpdateVertextBuffer(const void* data, size_t size, uint32_t bufferIndex = 0, uint32_t offset = 0);
		void CmdUpdateIndexBuffer(uint32_t* indices, size_t count, uint32_t bufferIndex = 0, uint32_t offset = 0);
		bool UpdateVulkanImageDescriptor(uint32_t bindingPoint, const VkDescriptorImageInfo& imageInfo, uint32_t descriptorSetIndex = 0);
#endif
		bool UpdateSamplers(const std::vector<Texture*>& textures, uint32_t bindingPoint, uint32_t descriptorSetIndex = 0);
		bool UpdateSampler(Texture* tetxure, uint32_t bindingPoint, uint32_t descriptorSetIndex = 0);
		bool UpdateSampler(Framebuffer* framebuffer, uint32_t bindingPoint, uint32_t attachmentIndex = 0, uint32_t descriptorSetIndex = 0);
		bool UpdateSampler(Framebuffer* framebuffer, uint32_t bindingPoint, const std::string& attachmentName, uint32_t descriptorSetIndex = 0);
		bool UpdateCubeMap(Texture* cubeMap, uint32_t bindingPoint, uint32_t descriptorSetIndex = 0);
		template<typename T>
		void SubmitUniform(const std::string& name, const void* data, uint32_t arrayElements = 0, size_t size = 0)
		{
#ifdef FROSTIUM_OPENGL_IMPL
			m_Shader->Bind();
			m_Shader->SumbitUniform<T>(name, data, arrayElements, size);
#endif
		}

#ifndef FROSTIUM_OPENGL_IMPL
		const VkPipeline& GetVkPipeline(DrawMode mode);
		const VulkanShader* GetVulkanShader();
		VkCommandBuffer GetVkCommandBuffer() const;
		void SetCommandBuffer(VkCommandBuffer cmdBuffer);
#else
		void BindOpenGLShader();
#endif
	private:

		// Helpres
		bool IsPipelineCreateInfoValid(const GraphicsPipelineCreateInfo* pipelineInfo);

	private:

#ifndef FROSTIUM_OPENGL_IMPL
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