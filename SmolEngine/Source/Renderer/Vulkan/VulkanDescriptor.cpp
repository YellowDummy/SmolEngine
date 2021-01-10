#include "stdafx.h"
#include "VulkanDescriptor.h"

#include "Renderer/Vulkan/VulkanShaderResources.h"
#include "Renderer/Vulkan/VulkanTexture.h"
#include "Renderer/Vulkan/VulkanContext.h"

VkWriteDescriptorSet SmolEngine::VulkanDescriptor::Create(VkDescriptorSet descriptorSet,
	uint32_t binding, VkDescriptorBufferInfo* descriptorBufferInfo, VkDescriptorType descriptorType)
{
	const auto& device = *VulkanContext::GetDevice().GetLogicalDevice();

	VkWriteDescriptorSet writeSet = {};
	{
		writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeSet.dstSet = descriptorSet;
		writeSet.dstBinding = binding;
		writeSet.dstArrayElement = 0; // temp
		writeSet.descriptorType = descriptorType;
		writeSet.descriptorCount = 1;
		writeSet.pBufferInfo = descriptorBufferInfo;
	}

	return writeSet;
}

VkWriteDescriptorSet SmolEngine::VulkanDescriptor::Create(VkDescriptorSet descriptorSet, uint32_t binding, VkDescriptorImageInfo* imageInfo, VkDescriptorType descriptorType)
{
	VkWriteDescriptorSet writeSet = {};
	{
		writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeSet.dstSet = descriptorSet;
		writeSet.descriptorType = descriptorType;
		writeSet.descriptorCount = 1;
		writeSet.dstArrayElement = 0;
		writeSet.dstBinding = binding;
		writeSet.pImageInfo = imageInfo;
	}

	return writeSet;
}

VkWriteDescriptorSet SmolEngine::VulkanDescriptor::Create(VkDescriptorSet descriptorSet, uint32_t binding, const std::vector<VkDescriptorImageInfo>& descriptorimageInfos, VkDescriptorType descriptorType)
{
	VkWriteDescriptorSet writeSet = {};
	{
		writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeSet.dstSet = descriptorSet;
		writeSet.descriptorType = descriptorType;
		writeSet.dstBinding = binding;
		writeSet.dstArrayElement = 0;
		writeSet.descriptorCount = static_cast<uint32_t>(descriptorimageInfos.size());
		writeSet.pImageInfo = descriptorimageInfos.data();
	}

	return writeSet;
}
