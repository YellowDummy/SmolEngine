#include "stdafx.h"
#include "VulkanIndexBuffer.h"

#include "Renderer/Vulkan/VulkanDevice.h"

namespace SmolEngine
{
	VulkanIndexBuffer::VulkanIndexBuffer()
	{

	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{

	}

	void VulkanIndexBuffer::Create(const uint32_t* data, uint64_t count)
	{
		m_ElementsCount = count;
		m_IndexBuffer.Create(data, sizeof(uint32_t) * count, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	}

	void VulkanIndexBuffer::Create(uint64_t size)
	{
		m_IndexBuffer.Create(size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	}

	void VulkanIndexBuffer::SetData(const uint32_t* data, uint64_t count)
	{
		m_ElementsCount = count;
		m_IndexBuffer.SetData(data, sizeof(uint32_t) * count);
	}

	void VulkanIndexBuffer::CmdUpdateData(VkCommandBuffer cmdBuffer, const void* data, size_t size, uint32_t offset)
	{
		m_IndexBuffer.CmdUpdateData(cmdBuffer, data, size, offset);
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

	uint32_t VulkanIndexBuffer::GetCount() const
	{
		return m_ElementsCount;
	}

	const VkBuffer& VulkanIndexBuffer::GetBuffer() const
	{
		return m_IndexBuffer.GetBuffer();
	}
}