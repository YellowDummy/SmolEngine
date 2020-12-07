#include "stdafx.h"
#include "VulkanDescriptor.h"

#include "Core/Renderer/Vulkan/VulkanShaderResources.h"
#include "Core/Renderer/Vulkan/VulkanContext.h"

SmolEngine::VulkanDescriptor::VulkanDescriptor()
{
	m_Device = &VulkanContext::GetDevice();
}

SmolEngine::VulkanDescriptor::~VulkanDescriptor()
{

}

VkDescriptorSet SmolEngine::VulkanDescriptor::Init(const VkDescriptorSetLayout& descriptorSetLayout, uint32_t binding, VkDescriptorBufferInfo* descriptorBufferInfo, VkDescriptorType descriptorType)
{
	VkResult result = VK_ERROR_UNKNOWN;
	VkDescriptorSet descriptorSet = nullptr;
	const auto& device = *m_Device->GetLogicalDevice();

	VkDescriptorPoolSize poolSize;
	{
		poolSize.type = descriptorType;
		poolSize.descriptorCount = 1;
	}

	VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
	{
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.pNext = nullptr;
		descriptorPoolInfo.poolSizeCount = 1;
		descriptorPoolInfo.pPoolSizes = &poolSize;
		descriptorPoolInfo.maxSets = 1;

		result = vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &m_DescriptorPool);
		assert(result == VK_SUCCESS);
	}

	VkDescriptorSetAllocateInfo allocateInfo = {};
	{
		allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocateInfo.descriptorPool = m_DescriptorPool;
		allocateInfo.descriptorSetCount = 1;
		allocateInfo.pSetLayouts = &descriptorSetLayout;

		result = vkAllocateDescriptorSets(device, &allocateInfo, &descriptorSet);
		assert(result == VK_SUCCESS);
	}

	VkWriteDescriptorSet writeSet = {};
	{
		writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeSet.dstSet = descriptorSet;
		writeSet.dstBinding = binding;
		writeSet.dstArrayElement = 0; // temp
		writeSet.descriptorType = descriptorType;
		writeSet.descriptorCount = 1;
		writeSet.pBufferInfo = descriptorBufferInfo;

		vkUpdateDescriptorSets(device, 1, &writeSet, 0, nullptr);
	}

	return descriptorSet;
}
