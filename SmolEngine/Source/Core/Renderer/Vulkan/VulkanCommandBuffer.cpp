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

	}

	bool VulkanCommandBuffer::Init(const VulkanDevice* device, const VulkanCommandPool* commandPool, const VulkanSwapchain* targetSwapchain)
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
				return true;
			}
		}

		return false;
	}
}