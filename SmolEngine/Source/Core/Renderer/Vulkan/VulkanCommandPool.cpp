#include "stdafx.h"
#include "VulkanCommandPool.h"

#include "Core/SLog.h"
#include "Core/Renderer/Vulkan/VulkanDevice.h"


namespace SmolEngine
{

	VulkanCommandPool::VulkanCommandPool()
	{

	}

	VulkanCommandPool::~VulkanCommandPool()
	{
		if (m_Device != nullptr)
		{
			vkDestroyCommandPool(m_Device->m_VkLogicalDevice, m_VkCommandPool, nullptr);
		}
	}

	void VulkanCommandPool::Init(VulkanDevice* device)
	{
		if (SetupCommandPool(device))
		{
			m_Device = device;
		}
	}

	void VulkanCommandPool::Reset()
	{
		vkResetCommandPool(*m_Device->GetLogicalDevice(), m_VkCommandPool, 0);
	}

	bool VulkanCommandPool::SetupCommandPool(const VulkanDevice* device)
	{
		VkCommandPoolCreateInfo poolInfo = {};
		{
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.queueFamilyIndex = device->m_DeviceQueueFamilyIndex;
			poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		}

		VkResult result = vkCreateCommandPool(device->m_VkLogicalDevice, &poolInfo, nullptr, &m_VkCommandPool);

		assert(result == VK_SUCCESS);

		return result == VK_SUCCESS;
	}

	const VkCommandPool* VulkanCommandPool::GetCommandPool() const
	{
		return &m_VkCommandPool;
	}
}
