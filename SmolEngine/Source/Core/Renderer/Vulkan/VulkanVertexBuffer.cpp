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
		m_VertexBuffer.Create(data, size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	}

	void VulkanVertexBuffer::Create(uint64_t size)
	{
		m_VertexBuffer.Create(size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
	}

	void VulkanVertexBuffer::SetData(const void* data, uint64_t size, uint32_t offset)
	{
		m_VertexBuffer.SetData(data, size, offset);
	}

	void VulkanVertexBuffer::UploadData(const void* data, uint64_t size, uint32_t offset)
	{
		m_VertexBuffer.UpdateData(data, size, offset);
	}

	void* VulkanVertexBuffer::MapMemory()
	{
		return m_VertexBuffer.MapMemory();
	}

	void VulkanVertexBuffer::UnMapMemory()
	{
		m_VertexBuffer.UnMapMemory();
	}

	void VulkanVertexBuffer::Destroy()
	{
		m_VertexBuffer.Destroy();
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