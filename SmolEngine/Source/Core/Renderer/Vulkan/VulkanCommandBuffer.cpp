#include "stdafx.h"
#include "VulkanCommandBuffer.h"

#include "Core/Renderer/Vulkan/VulkanDevice.h"
#include "Core/Renderer/Vulkan/VulkanSwapchain.h"
#include "Core/Renderer/Vulkan/VulkanCommandPool.h"
#include "Core/Renderer/Vulkan/VulkanContext.h"

namespace SmolEngine
{
	VulkanCommandBuffer::VulkanCommandBuffer()
	{

	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
		//vkFreeCommandBuffers(*m_Device->GetLogicalDevice(), *m_CommandPool->GetCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
	}

	bool VulkanCommandBuffer::Init(VulkanDevice* device, VulkanCommandPool* commandPool, VulkanSwapchain* targetSwapchain)
	{
		m_CommandBuffers.resize(targetSwapchain->m_ImageCount);

		VkCommandBufferAllocateInfo commandBufferInfo = {};
		{
			commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferInfo.commandPool = commandPool->GetCommandPool();
			commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

			VkResult result = vkAllocateCommandBuffers(device->GetLogicalDevice(), &commandBufferInfo, m_CommandBuffers.data());
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

		vkFreeCommandBuffers(m_Device->GetLogicalDevice(), m_CommandPool->GetCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
		return Init(m_Device, m_CommandPool, m_TargetSwapchain);
	}

	const VkCommandBuffer VulkanCommandBuffer::GetVkCommandBuffer() const
	{
		return m_CommandBuffers[m_TargetSwapchain->GetCurrentBufferIndex()];
	}

	const VkCommandBuffer VulkanCommandBuffer::CreateSingleCommandBuffer(bool oneCommand)
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = VulkanContext::GetCommandPool().GetCommandPool();
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		VK_CHECK_RESULT(vkAllocateCommandBuffers(VulkanContext::GetDevice().GetLogicalDevice(), &allocInfo, &commandBuffer));

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (oneCommand)
		{
			beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		}

		VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffer, &beginInfo));

		return commandBuffer;
	}

	void VulkanCommandBuffer::EndSingleCommandBuffer(const VkCommandBuffer cmdBuffer)
	{
		vkEndCommandBuffer(cmdBuffer);

		VkSubmitInfo submitInfo = {};
		{
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &cmdBuffer;
		}

		
		VkQueue q = VulkanContext::GetDevice().GetQueue();
		vkQueueSubmit(q, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(q);
		vkFreeCommandBuffers(VulkanContext::GetDevice().GetLogicalDevice(), VulkanContext::GetCommandPool().GetCommandPool(), 1, &cmdBuffer);
	}

	void VulkanCommandBuffer::FlushCommandBuffer(const VkCommandBuffer cmdBuffer)
	{
		const uint64_t time_out = 100000000000;
		assert(cmdBuffer != VK_NULL_HANDLE);

		VK_CHECK_RESULT(vkEndCommandBuffer(cmdBuffer));
		VkSubmitInfo submitInfo = {};
		{
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &cmdBuffer;
		}

		VkFenceCreateInfo fenceCI = {};
		{
			fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCI.flags = 0;
		}

		VkFence fence;
		VK_CHECK_RESULT(vkCreateFence(VulkanContext::GetDevice().GetLogicalDevice(), &fenceCI, nullptr, &fence));
		VK_CHECK_RESULT(vkQueueSubmit(VulkanContext::GetDevice().GetQueue(), 1, &submitInfo, fence));
		VK_CHECK_RESULT(vkWaitForFences(VulkanContext::GetDevice().GetLogicalDevice(), 1, &fence, VK_TRUE, time_out));

		vkDestroyFence(VulkanContext::GetDevice().GetLogicalDevice(), fence, nullptr);
		vkFreeCommandBuffers(VulkanContext::GetDevice().GetLogicalDevice(), VulkanContext::GetCommandPool().GetCommandPool(), 1, &cmdBuffer);
	}

	size_t VulkanCommandBuffer::GetBufferSize() const
	{
		return m_CommandBuffers.size();
	}
}