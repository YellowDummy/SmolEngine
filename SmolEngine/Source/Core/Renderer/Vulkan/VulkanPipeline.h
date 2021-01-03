#pragma once
#include "Core/Core.h"
#include "Core/Renderer/Vulkan/Vulkan.h"
#include "Core/Renderer/Vulkan/VulkanDescriptor.h"

namespace SmolEngine
{
	class VulkanShader;
	class VulkanTexture;
	enum class DrawMode : uint16_t;
	struct VulkanPipelineSpecification;

	class VulkanPipeline
	{
	public:

		VulkanPipeline();

		~VulkanPipeline();

		/// 
		/// Main
		/// 
		
		bool Invalidate(VulkanPipelineSpecification* pipelineSpec);

		bool CreatePipeline(DrawMode mode);

		bool ReCreate();

		void Destroy();

		void UpdateSamplers2D(const std::vector<VulkanTexture*>& textures, VkCommandBuffer cmdBuffer, uint32_t setIndex = 0);

		/// 
		/// Getters
		/// 

		const VkPipeline& GetVkPipeline(DrawMode mode);

		const VkPipelineLayout& GetVkPipelineLayot() const;

		const VkDescriptorSet GetVkDescriptorSet(uint32_t setIndex = 0) const;

		const VkDescriptorSetLayout GetVkDescriptorSetLayout() const;

	private:

		void BuildDescriptors(VulkanShader* shader, const std::vector<VulkanTexture*>& textures, uint32_t DescriptorSets);

		VkPrimitiveTopology GetVkTopology(DrawMode mode);

		VkPolygonMode GetVkPolygonMode(DrawMode mode);

	private:

		std::vector<VulkanTexture*> m_ReservedTextures;
		std::vector<std::vector<VkWriteDescriptorSet>> m_WriteDescriptorSets;

		std::vector<VkDescriptorSet> m_DesciptorSets;
		VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;

		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

		std::unordered_map<DrawMode, VkPipelineCache> m_PipelineCaches;
		std::unordered_map<DrawMode, VkPipeline> m_Pipelines;

		VkDescriptorPool m_DescriptorPool = nullptr;
		VulkanPipelineSpecification* m_VulkanPipelineSpecification = nullptr;
		
	};
}