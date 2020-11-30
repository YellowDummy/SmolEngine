#include "stdafx.h"
#include "VulkanDescriptor.h"

#include "Core/Renderer/Vulkan/VulkanContext.h"
#include "Core/Renderer/Vulkan/VulkanBuffer.h"

namespace SmolEngine
{
	VulkanDescriptor::VulkanDescriptor()
	{

	}

	VulkanDescriptor::~VulkanDescriptor()
	{
		const auto& device = *VulkanContext::GetDevice().GetLogicalDevice();

		vkDestroyDescriptorSetLayout(device, m_SetLayot, nullptr);
		vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
	}

	Ref<VulkanDescriptor> VulkanDescriptor::Create(const Ref<VulkanBuffer>& buffer, VkDescriptorType type, VkShaderStageFlagBits shaderStage, uint32_t maxSets, uint32_t binding, uint32_t offset)
	{
		const uint32_t descriptorCount = 1;
		const auto& device = *VulkanContext::GetDevice().GetLogicalDevice();

		Ref<VulkanDescriptor> descriptor = std::make_shared<VulkanDescriptor>();
		{
			VkDescriptorPoolSize poolSize;
			{
				poolSize.type = type;
				poolSize.descriptorCount = descriptorCount;
			}

			VkDescriptorSetLayoutBinding layoutBinding = {};
			{
				layoutBinding.binding = binding;
				layoutBinding.descriptorType = type;
				layoutBinding.descriptorCount = descriptorCount;
				layoutBinding.stageFlags = shaderStage;
			}

			VkDescriptorSetLayoutCreateInfo layoutInfo = {};
			{
				layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layoutInfo.bindingCount = 1; // temp
				layoutInfo.pBindings = &layoutBinding;


				VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptor->m_SetLayot);
				assert(result == VK_SUCCESS);
			}

			VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
			{
				descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
				descriptorPoolInfo.poolSizeCount = 1; // temp
				descriptorPoolInfo.pPoolSizes = &poolSize;
				descriptorPoolInfo.maxSets = maxSets;

				VkResult result = vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &descriptor->m_DescriptorPool);
				assert(result == VK_SUCCESS);
			}

			VkDescriptorSetAllocateInfo allocateInfo = {};
			{
				allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				allocateInfo.descriptorPool = descriptor->m_DescriptorPool;
				allocateInfo.descriptorSetCount = maxSets;
				allocateInfo.pSetLayouts = &descriptor->m_SetLayot;

				VkResult result = vkAllocateDescriptorSets(device, &allocateInfo, &descriptor->m_Set);
				assert(result == VK_SUCCESS);
			}

			VkDescriptorBufferInfo bufferInfo = {};
			{
				bufferInfo.buffer = buffer->GetBuffer();
				bufferInfo.offset = offset;
				bufferInfo.range = buffer->GetSize();
			}

			VkWriteDescriptorSet writeSet = {};
			{
				writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writeSet.dstSet = descriptor->m_Set;
				writeSet.dstBinding = binding;
				writeSet.dstArrayElement = 0; // temp
				writeSet.descriptorType = type;
				writeSet.descriptorCount = descriptorCount;
				writeSet.pBufferInfo = &bufferInfo;

				vkUpdateDescriptorSets(device, 1, &writeSet, 0, nullptr);
			}
		}

		return descriptor;
	}
}