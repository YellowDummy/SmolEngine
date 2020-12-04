#pragma once
#include "Core/Core.h"

#include "Core/Renderer/Vulkan/Vulkan.h"

#include <vector>

namespace SmolEngine
{
	class VulkanDevice;

	class VulkanShader;

	class VulkanDescriptor
	{
	public:

		VulkanDescriptor();

		~VulkanDescriptor();

		///
		///  Main
		/// 
		
		VkDescriptorSet Init(const VkDescriptorSetLayout& descriptorSetLayout, uint32_t binding, VkDescriptorBufferInfo* descriptorBufferInfo,
			VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

		/// 
		/// Getter
		/// 



	private:

		VkWriteDescriptorSet m_WriteSet = {};

		VkDescriptorPool m_DescriptorPool = nullptr;

		VulkanDevice* m_Device;
	};
}