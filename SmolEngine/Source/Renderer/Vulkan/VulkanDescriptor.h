#pragma once
#include "Core/Core.h"

#include "Renderer/Vulkan/Vulkan.h"

#include <vector>

namespace SmolEngine
{
	class VulkanDevice;
	class VulkanTexture;
	class VulkanShader;

	class VulkanDescriptor
	{
	public:

		static VkWriteDescriptorSet Create(VkDescriptorSet descriptorSet, uint32_t binding, VkDescriptorBufferInfo* descriptorBufferInfo,
			VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

		static VkWriteDescriptorSet Create(VkDescriptorSet descriptorSet, uint32_t binding, VkDescriptorImageInfo* imageInfo,
			VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

		static VkWriteDescriptorSet Create(VkDescriptorSet descriptorSet, uint32_t binding, const std::vector<VkDescriptorImageInfo>& descriptorimageInfos,
			VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

	};
}