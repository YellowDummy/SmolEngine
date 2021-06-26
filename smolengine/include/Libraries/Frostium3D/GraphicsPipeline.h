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
#include "Common/Common.h"

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	class Framebuffer;
	class CubeMap;
	class Mesh;
	class Shader;
	class Texture;

	class GraphicsPipeline
	{
	public:
		GraphicsPipeline() = default;
		~GraphicsPipeline();

		PipelineCreateResult Create(GraphicsPipelineCreateInfo* pipelineInfo);
		PipelineCreateResult Reload();

		void ClearColors(const glm::vec4& clearColors = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		void BeginRenderPass(bool flip = false);
		void EndRenderPass();
		void BeginCommandBuffer(bool isMainCmdBufferInUse = false);
		void EndCommandBuffer();
		void ResetStates();
		void Destroy();

		void DrawIndexed(uint32_t vbIndex = 0, uint32_t ibIndex = 0);
		void DrawIndexed(VertexBuffer* vb, IndexBuffer* ib);
		void Draw(VertexBuffer* vb, uint32_t vertextCount);
		void Draw(uint32_t vertextCount, uint32_t vertexBufferIndex = 0);
		void DrawMeshIndexed(Mesh* mesh, uint32_t instances = 1);
		void DrawMesh(Mesh* mesh, uint32_t instances = 1);

		bool SubmitBuffer(uint32_t bindingPoint, size_t size, const void* data, uint32_t offset = 0);
		void SubmitPushConstant(ShaderType shaderStage, size_t size, const void* data);
		void SetVertexBuffers(const std::vector<Ref<VertexBuffer>>& buffer);
		void SetIndexBuffers(const std::vector<Ref<IndexBuffer>>& buffer);
#ifndef FROSTIUM_OPENGL_IMPL
		void CmdUpdateVertextBuffer(const void* data, size_t size, uint32_t bufferIndex = 0, uint32_t offset = 0);
		void CmdUpdateIndexBuffer(uint32_t* indices, size_t count, uint32_t bufferIndex = 0, uint32_t offset = 0);
		bool UpdateVulkanImageDescriptor(uint32_t bindingPoint, const VkDescriptorImageInfo& imageInfo);
#endif
		bool UpdateSamplers(const std::vector<Texture*>& textures, uint32_t bindingPoint);
		bool UpdateSampler(Texture* tetxure, uint32_t bindingPoint);
		bool UpdateSampler(Framebuffer* framebuffer, uint32_t bindingPoint, uint32_t attachmentIndex = 0);
		bool UpdateSampler(Framebuffer* framebuffer, uint32_t bindingPoint, const std::string& attachmentName);
		bool UpdateCubeMap(Texture* cubeMap, uint32_t bindingPoint);

		void SetDescriptorIndex(uint32_t value);
		void SetDrawMode(DrawMode mode);
		void SetFramebufferIndex(uint32_t index);
		void SetFramebufferAttachmentIndex(uint32_t index);

#ifndef FROSTIUM_OPENGL_IMPL
		const VkPipeline& GetVkPipeline(DrawMode mode);
		const VulkanShader* GetVulkanShader();
		VkCommandBuffer GetVkCommandBuffer() const;
		void SetCommandBuffer(VkCommandBuffer cmdBuffer);
#else
		void BindOpenGLShader();
#endif
	private:
		bool IsPipelineCreateInfoValid(const GraphicsPipelineCreateInfo* pipelineInfo);

	private:

#ifndef FROSTIUM_OPENGL_IMPL
		CommandBufferStorage              m_CmdStorage{};
		VulkanPipeline                    m_VulkanPipeline = {};
		VkCommandBuffer                   m_CommandBuffer = nullptr;
		bool                              m_IsMainCmdBufferInUse = false;
#endif
		Ref<VertexArray>                  m_VextexArray = nullptr;
		Ref<Shader>                       m_Shader = nullptr;
		GraphicsContext*                  m_GraphicsContext = nullptr;
		DrawMode                          m_DrawMode = DrawMode::Triangle;
		uint32_t                          m_DescriptorIndex = 0;
		uint32_t                          m_FBIndex = 0;
		uint32_t                          m_FBattachmentIndex = 0;
		GraphicsPipelineCreateInfo        m_PiplineCreateInfo;
		std::vector<Ref<VertexBuffer>>    m_VertexBuffers;
		std::vector<Ref<IndexBuffer>>     m_IndexBuffers;
	};
}