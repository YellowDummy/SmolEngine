#include "stdafx.h"
#include "VulkanDescriptor.h"

#include "Core/Renderer/Vulkan/VulkanShaderResources.h"
#include "Core/Renderer/Vulkan/VulkanContext.h"

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
	const auto& device = *VulkanContext::GetDevice().GetLogicalDevice();

	VkWriteDescriptorSet writeSet = {};
	{
		writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeSet.dstSet = descriptorSet;
		writeSet.descriptorType = descriptorType;
		writeSet.dstBinding = binding;
		writeSet.pImageInfo = imageInfo;
		writeSet.descriptorCount = 1;
	}

	return writeSet;
}
