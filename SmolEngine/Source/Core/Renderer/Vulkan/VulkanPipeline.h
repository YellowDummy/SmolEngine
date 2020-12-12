#pragma once
#include "Core/Core.h"
#include "Core/Renderer/Vulkan/Vulkan.h"
#include "Core/Renderer/Vulkan/VulkanDescriptor.h"

namespace SmolEngine
{
	class VulkanShader;
	class VulkanTexture;
	struct VulkanPipelineSpecification;

	class VulkanPipeline
	{
	public:

		VulkanPipeline();

		~VulkanPipeline();

		/// 
		/// Main
		/// 
		
		bool Invalidate(const VulkanPipelineSpecification* pipelineSpec);

		bool ReCreate();

		void Destroy();

		void UpdateSamplers2D(const std::vector<VulkanTexture*>& textures);

		/// 
		/// Getters
		/// 

		const VkPipeline& GetVkPipeline() const;

		const VkPipelineLayout& GetVkPipelineLayot() const;

		const VkDescriptorSet GetVkDescriptorSet() const;

		const VkDescriptorSetLayout GetVkDescriptorSetLayout() const;

	private:

		void BuildDescriptors(VulkanShader* shader, const std::vector<VulkanTexture*>& textures);

	private:

		std::vector<VkWriteDescriptorSet> m_WriteDescriptorSets;

		VkDescriptorSet m_DesciptorSet = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;

		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;
		VkPipeline m_Pipeline = VK_NULL_HANDLE;

		VkDescriptorPool m_DescriptorPool = nullptr;
		VulkanPipelineSpecification* m_VulkanPipelineSpecification = nullptr;
		
	};
}