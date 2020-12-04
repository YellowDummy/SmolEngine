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
		SetData(data, size);
	}

	void VulkanIndexBuffer::SetData(const void* data, uint64_t size, uint32_t offset)
	{
		m_IndexBuffer.Create(data, size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, offset);
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