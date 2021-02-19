#include "stdafx.h"
#include "VulkanBufferPool.h"

#include "Core/SLog.h"

namespace SmolEngine
{
	VulkanBufferPool* VulkanBufferPool::s_Instance = new VulkanBufferPool();

	void VulkanBufferPool::Add(size_t size, uint32_t binding, VkMemoryPropertyFlags mem, VkBufferUsageFlags usage, 
		VkDescriptorBufferInfo& outDescriptorBufferInfo)
	{
		auto& it = m_Buffers.find(binding);
		if (it == m_Buffers.end())
		{
			Ref<BufferObject> object = std::make_shared<BufferObject>();
			object->VkBuffer.Create(size, mem, usage);
			object->DesriptorBufferInfo.buffer = object->VkBuffer.GetBuffer();
			object->DesriptorBufferInfo.offset = 0;
			object->DesriptorBufferInfo.range = size;
			m_Buffers[binding] = object;

			outDescriptorBufferInfo = object->DesriptorBufferInfo;
			return;
		}

		NATIVE_INFO("UBO/SSBO with binding {} is reused", binding);
		outDescriptorBufferInfo = it->second->DesriptorBufferInfo;
	}

	bool VulkanBufferPool::IsBindingExist(uint32_t binding)
	{
		return m_Buffers.find(binding) != m_Buffers.end();
	}

	VulkanBuffer* VulkanBufferPool::GetBuffer(uint32_t binding)
	{
		auto& it = m_Buffers.find(binding);
		if (it == m_Buffers.end())
			return nullptr;

		return &it->second->VkBuffer;
	}

	VulkanBufferPool* VulkanBufferPool::GetSingleton()
	{
		return s_Instance;
	}
}