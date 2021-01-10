#include "stdafx.h"
#include "VulkanStagingBuffer.h"

namespace SmolEngine
{
	VulkanStagingBuffer::VulkanStagingBuffer()
	{

	}

	VulkanStagingBuffer::~VulkanStagingBuffer()
	{

	}

	void VulkanStagingBuffer::Create(const void* data, uint64_t size)
	{
		m_StagingBuffer.Create(data, size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	}

	void VulkanStagingBuffer::Create(uint64_t size)
	{
		m_StagingBuffer.Create(size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	}

	void VulkanStagingBuffer::SetData(const void* data, uint64_t size, uint32_t offset)
	{
		m_StagingBuffer.SetData(data, size, offset);
	}

	void* VulkanStagingBuffer::MapMemory()
	{
		return m_StagingBuffer.MapMemory();
	}

	void VulkanStagingBuffer::UnMapMemory()
	{
		m_StagingBuffer.UnMapMemory();
	}

	void VulkanStagingBuffer::Destroy()
	{
		m_StagingBuffer.Destroy();
	}

	uint32_t VulkanStagingBuffer::GetSize() const
	{
		return m_StagingBuffer.GetSize();
	}

	const VkBuffer& VulkanStagingBuffer::GetBuffer() const
	{
		return m_StagingBuffer.GetBuffer();
	}
}