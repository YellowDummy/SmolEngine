#pragma once
#include "Core/Core.h"

#include <vulkan/vulkan.h>

namespace SmolEngine
{
	class VulkanBuffer;

	class VulkanDescriptor
	{
	public:

		VulkanDescriptor();

		~VulkanDescriptor();

		/// 
		/// Factory
		/// 
		
		static Ref<VulkanDescriptor> Create(const Ref<VulkanBuffer>& buffer, VkDescriptorType type, VkShaderStageFlagBits shaderStage, uint32_t maxSets, uint32_t binding, uint32_t offset = 0);

	private:

		VkDescriptorPool m_DescriptorPool = nullptr;
		VkDescriptorSetLayout m_SetLayot = nullptr;
		VkDescriptorSet m_Set = nullptr;

		VkWriteDescriptorSet m_WriteSet = {};
	};
}