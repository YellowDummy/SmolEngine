#include "stdafx.h"
#include "VulkanSemaphore.h"

#include "Core/Renderer/Vulkan/VulkanCommandBuffer.h"
#include "Core/Renderer/Vulkan/VulkanDevice.h"

namespace SmolEngine
{
    VulkanSemaphore::VulkanSemaphore()
    {

    }

    VulkanSemaphore::~VulkanSemaphore()
    {

    }

    bool VulkanSemaphore::Init(const VulkanDevice* device, const VulkanCommandBuffer* commandBuffer)
    {
        VkSemaphoreCreateInfo semaphoreCI = {};
        {
            semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VK_CHECK_RESULT(vkCreateSemaphore(*device->GetLogicalDevice(), &semaphoreCI, nullptr, &m_PresentComplete));
            VK_CHECK_RESULT(vkCreateSemaphore(*device->GetLogicalDevice(), &semaphoreCI, nullptr, &m_RenderComplete));
        }

        VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        m_SubmitInfo = {};
        {
            m_SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            m_SubmitInfo.pWaitDstStageMask = &pipelineStageFlags;
            m_SubmitInfo.waitSemaphoreCount = 1;
            m_SubmitInfo.pWaitSemaphores = &m_PresentComplete;
            m_SubmitInfo.signalSemaphoreCount = 1;
            m_SubmitInfo.pSignalSemaphores = &m_RenderComplete;

        }

        VkFenceCreateInfo fenceCreateInfo = {};
        {
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            m_WaitFences.resize(commandBuffer->GetBufferSize());

            for (auto& fence : m_WaitFences)
            {
                auto create = vkCreateFence(*device->GetLogicalDevice(), &fenceCreateInfo, nullptr, &fence);
                VK_CHECK_RESULT(create);

                if (create == VK_SUCCESS)
                {
                    continue;
                }

                return false;
            }
        }

        return true;
    }

    const VkSemaphore VulkanSemaphore::GetPresentCompleteSemaphore() const
    {
        return m_PresentComplete;
    }

    const VkSemaphore VulkanSemaphore::GetRenderCompleteSemaphore() const
    {
        return m_RenderComplete;
    }

    const std::vector<VkFence>& VulkanSemaphore::GetVkFences() const
    {
        return m_WaitFences;
    }

    const VkSubmitInfo* VulkanSemaphore::GetSubmitInfo() const
    {
        return &m_SubmitInfo;
    }
}