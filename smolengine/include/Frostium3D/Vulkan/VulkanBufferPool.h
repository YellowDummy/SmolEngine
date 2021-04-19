#pragma once
#ifndef FROSTIUM_OPENGL_IMPL
#include "Common/Core.h"

#include "Vulkan/Vulkan.h"
#include "Vulkan/VulkanBuffer.h"

namespace Frostium
{
	struct BufferObject
	{
		VkDescriptorBufferInfo DesriptorBufferInfo;
		VulkanBuffer VkBuffer = {};
	};

	class VulkanBufferPool
	{
	public:

		void Add(size_t size, uint32_t binding, VkMemoryPropertyFlags mem, VkBufferUsageFlags usage,
			VkDescriptorBufferInfo& outDescriptorBufferInfo);
		bool IsBindingExist(uint32_t binding);

		// Getters

		VulkanBuffer* GetBuffer(uint32_t binding);
		static VulkanBufferPool* GetSingleton();

	private:

		static VulkanBufferPool*                           s_Instance;
		std::unordered_map<uint32_t, Ref<BufferObject>>    m_Buffers;
	};
}
#endif