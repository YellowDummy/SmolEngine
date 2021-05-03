#pragma once
#ifndef FROSTIUM_OPENGL_IMPL
#include "Common/Core.h"

#include "Vulkan/Vulkan.h"
#include "Vulkan/VulkanDescriptor.h"
#include "Common/BufferElement.h"

namespace Frostium
{
	class VulkanShader;
	class VulkanTexture;
	class VulkanShader;
	enum class DrawMode : uint16_t;
	enum class CullMode : uint16_t;
	enum class BlendFactor : uint16_t;
	enum class BlendOp : uint16_t;

	struct GraphicsPipelineCreateInfo;

	class VulkanPipeline
	{
	public:
		VulkanPipeline();
		~VulkanPipeline();

		// Main
		bool Invalidate(GraphicsPipelineCreateInfo* pipelineSpec, VulkanShader* shader);
		bool CreatePipeline(DrawMode mode);
		bool ReCreate();
		void Destroy();

		// Update Descriptors
		bool UpdateSamplers2D(const std::vector<VulkanTexture*>& textures, uint32_t bindingPoint, uint32_t setIndex = 0);
		bool UpdateCubeMap(const VulkanTexture* cubeMap, uint32_t bindingPoint, uint32_t setIndex = 0);

		// Save / Load
		bool SaveCache(const std::string& fileName, DrawMode mode);
		bool CreateOrLoadCached(const std::string& fileName, DrawMode mode);

		// Getters
		const VkPipeline& GetVkPipeline(DrawMode mode);
		const VkPipelineLayout& GetVkPipelineLayot() const;
		const VkDescriptorSet GetVkDescriptorSets(uint32_t setIndex = 0) const;
	private:

		bool IsBlendEnableEnabled();
		void BuildDescriptors(VulkanShader* shader, uint32_t DescriptorSets);
		VkFormat GetVkInputFormat(DataTypes type);
		VkPrimitiveTopology GetVkTopology(DrawMode mode);
		VkPolygonMode GetVkPolygonMode(DrawMode mode);
		VkCullModeFlags GetVkCullMode(CullMode mode);
		VkBlendFactor GetVkBlendFactor(BlendFactor factor);
		VkBlendOp GetVkBlendOp(BlendOp op);

	private:
		VkPipelineLayout                                m_PipelineLayout = VK_NULL_HANDLE;
		VkRenderPass                                    m_TargetRenderPass = nullptr;
		VkDevice                                        m_Device = nullptr;
		VulkanShader* m_Shader = nullptr;

		VkDescriptorPool                                m_DescriptorPool = nullptr;
		GraphicsPipelineCreateInfo* m_PipelineSpecification = nullptr;

		std::vector<VulkanDescriptor>                   m_Descriptors;
		std::vector<VkDescriptorSetLayout>              m_SetLayout;
		std::unordered_map<DrawMode, VkPipelineCache>   m_PipelineCaches;
		std::unordered_map<DrawMode, VkPipeline>        m_Pipelines;
		std::string                                     m_FilePath = "";

	private:

		friend class GraphicsPipeline;
	};
}
#endif