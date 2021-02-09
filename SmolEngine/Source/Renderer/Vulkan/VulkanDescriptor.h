#pragma once
#include "Renderer/Vulkan/Vulkan.h"

namespace SmolEngine
{
	class VulkanShader;
	class VulkanTexture;

	class VulkanDescriptor
	{
	public:

		VulkanDescriptor();

		~VulkanDescriptor();

		void GenDescriptorSet(VulkanShader* shader, VkDescriptorPool pool);

		void GenUniformBuffersDescriptors(VulkanShader* shader);

		void GenStorageBufferDescriptors(VulkanShader* shader);

		void GenSamplersDescriptors(VulkanShader* shader);

		// Update

		bool Update2DSamplers(const std::vector<VulkanTexture*>& textures, uint32_t bindingPoint);

		bool UpdateImageResource(uint32_t bindingPoint, const VkDescriptorImageInfo& imageInfo);

		bool UpdateCubeMap(const VulkanTexture* cubeMap, uint32_t bindingPoint);

		void UpdateWriteSets();

		// Getters

		const VkDescriptorSet GetDescriptorSets() const;

	private:

		VkWriteDescriptorSet CreateWriteSet(VkDescriptorSet descriptorSet, uint32_t binding, 
			VkDescriptorBufferInfo* descriptorBufferInfo,
			VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

		VkWriteDescriptorSet CreateWriteSet(VkDescriptorSet descriptorSet, uint32_t binding,
			VkDescriptorImageInfo* imageInfo,
			VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

		VkWriteDescriptorSet CreateWriteSet(VkDescriptorSet descriptorSet, uint32_t binding,
			const std::vector<VkDescriptorImageInfo>& descriptorimageInfos,
			VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

	private:

		VkDescriptorSetLayout                   m_DescriptorSetLayout;
		VkDescriptorSet                         m_DescriptorSet;
		VkDevice                                m_Device;

		std::vector<VkWriteDescriptorSet>       m_WriteSets;
		VkDescriptorImageInfo                   m_ImageInfo;

	private:

		friend class VulkanPipeline;
	};
}