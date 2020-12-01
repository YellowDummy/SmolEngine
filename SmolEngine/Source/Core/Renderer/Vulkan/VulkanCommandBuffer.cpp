#include "stdafx.h"
#include "VulkanCommandBuffer.h"

#include "Core/Renderer/Vulkan/VulkanDevice.h"
#include "Core/Renderer/Vulkan/VulkanSwapchain.h"
#include "Core/Renderer/Vulkan/VulkanCommandPool.h"

namespace SmolEngine
{
	VulkanCommandBuffer::VulkanCommandBuffer()
	{

	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
		vkFreeCommandBuffers(*m_Device->GetLogicalDevice(), *m_CommandPool->GetCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
	}

	bool VulkanCommandBuffer::Init(VulkanDevice* device, VulkanCommandPool* commandPool, VulkanSwapchain* targetSwapchain)
	{
		m_CommandBuffers.resize(targetSwapchain->m_ImageCount);

		VkCommandBufferAllocateInfo commandBufferInfo = {};
		{
			commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferInfo.commandPool = *commandPool->GetCommandPool();
			commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

			VkResult result = vkAllocateCommandBuffers(*device->GetLogicalDevice(), &commandBufferInfo, m_CommandBuffers.data());
			VK_CHECK_RESULT(result);

			if (result == VK_SUCCESS)
			{
				m_CommandPool = commandPool;
				m_Device = device;
				m_TargetSwapchain = targetSwapchain;

				return true;
			}
		}

		return false;
	}

	bool VulkanCommandBuffer::Recrate()
	{
		if (!m_Device || !m_CommandPool || !m_TargetSwapchain)
		{
			return false;
		}

		vkFreeCommandBuffers(*m_Device->GetLogicalDevice(), *m_CommandPool->GetCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
		return Init(m_Device, m_CommandPool, m_TargetSwapchain);
	}

	const std::vector<VkCommandBuffer>& VulkanCommandBuffer::GetCommandBuffer() const
	{
		return m_CommandBuffers;
	}

	size_t VulkanCommandBuffer::GetBufferSize() const
	{
		return m_CommandBuffers.size();
	}
}