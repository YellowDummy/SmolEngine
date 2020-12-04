#include "stdafx.h"
#include "VulkanVertexBuffer.h"

namespace SmolEngine
{
	VulkanVertexBuffer::VulkanVertexBuffer()
	{

	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{

	}

	void VulkanVertexBuffer::Create(const void* data, uint64_t size)
	{
		SetData(data, size);
	}

	void VulkanVertexBuffer::SetData(const void* data, uint64_t size, uint32_t offset)
	{
		m_VertexBuffer.Create(data, size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, offset);
	}

	uint32_t VulkanVertexBuffer::GetSize() const
	{
		return m_VertexBuffer.GetSize();
	}

	const VkBuffer& VulkanVertexBuffer::GetBuffer() const
	{
		return m_VertexBuffer.GetBuffer();
	}
}