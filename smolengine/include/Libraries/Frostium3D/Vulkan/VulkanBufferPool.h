#pragma once
#ifndef FROSTIUM_OPENGL_IMPL
#include "Common/Core.h"

#include "Vulkan/Vulkan.h"
#include "Vulkan/VulkanBuffer.h"

#include <mutex>

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

		void Add(size_t size, uint32_t binding, VkBufferUsageFlags usage, VkDescriptorBufferInfo& outDescriptorBufferInfo, bool isStatic = false, void* data = nullptr);
		bool IsBindingExist(uint32_t binding);

		// Getters

		VulkanBuffer* GetBuffer(uint32_t binding);
		static VulkanBufferPool* GetSingleton();

	private:

		static VulkanBufferPool*                           s_Instance;
#ifdef FROSTIUM_SMOLENGINE_IMPL
		std::mutex                                         m_Mutex{};
#endif
		std::unordered_map<uint32_t, Ref<BufferObject>>    m_Buffers;
	};
}
#endif