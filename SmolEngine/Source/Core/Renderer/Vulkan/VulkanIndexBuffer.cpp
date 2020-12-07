#include "stdafx.h"
#include "VulkanIndexBuffer.h"

#include "Core/Renderer/Vulkan/VulkanDevice.h"

namespace SmolEngine
{
	VulkanIndexBuffer::VulkanIndexBuffer()
	{

	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{

	}

	void VulkanIndexBuffer::Create(const void* data, uint64_t size)
	{
		m_IndexBuffer.Create(data, size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	}

	void VulkanIndexBuffer::Create(uint64_t size)
	{
		m_IndexBuffer.Create(size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	}

	void VulkanIndexBuffer::SetData(const void* data, uint64_t size, uint32_t offset)
	{
		m_IndexBuffer.SetData(data, size, offset);
	}

	void* VulkanIndexBuffer::MapMemory()
	{
		return m_IndexBuffer.MapMemory();
	}

	void VulkanIndexBuffer::UnMapMemory()
	{
		m_IndexBuffer.UnMapMemory();
	}

	void VulkanIndexBuffer::Destroy()
	{
		m_IndexBuffer.Destroy();
	}

	uint32_t VulkanIndexBuffer::GetSize() const
	{
		return m_IndexBuffer.GetSize();
	}

	const VkBuffer& VulkanIndexBuffer::GetBuffer() const
	{
		return m_IndexBuffer.GetBuffer();
	}
}