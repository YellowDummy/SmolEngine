#include "stdafx.h"
#include "VulkanSwapchainFramebuffer.h"

#include "Renderer/Vulkan/VulkanDevice.h"
#include "Renderer/Vulkan/VulkanSwapchain.h"

namespace SmolEngine
{
	VulkanSwapchainFramebuffer::VulkanSwapchainFramebuffer()
	{

	}

	VulkanSwapchainFramebuffer::~VulkanSwapchainFramebuffer()
	{

	}

	VkResult VulkanSwapchainFramebuffer::Init(VulkanSwapchain* swapchain, uint32_t width, uint32_t height)
	{
		m_Swapchain = swapchain;

		return Create(width, height);
	}

	VkResult VulkanSwapchainFramebuffer::Create(uint32_t width, uint32_t height)
	{
		VkResult result = VK_ERROR_UNKNOWN;
		VkImageView ivAttachment[2];

		// Depth attachment is the same for all framebuffers

		ivAttachment[1] = m_Swapchain->m_DepthStencil.ImageView;

		VkFramebufferCreateInfo framebufferCI = {};
		{
			framebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCI.pNext = NULL;
			framebufferCI.renderPass = m_Swapchain->m_RenderPass;
			framebufferCI.attachmentCount = 2;
			framebufferCI.pAttachments = ivAttachment;
			framebufferCI.width = width;
			framebufferCI.height = height;
			framebufferCI.layers = 1;
		}

		m_Framebuffers.resize(m_Swapchain->m_ImageCount);

		// Create framebuffer for every swapchain image

		const auto& logicalDevice = m_Swapchain->m_Device->GetLogicalDevice();

		for (uint32_t i = 0; i < m_Framebuffers.size(); ++i)
		{
			ivAttachment[0] = m_Swapchain->m_Buffers[i].View;
			result = vkCreateFramebuffer(logicalDevice, &framebufferCI, nullptr, &m_Framebuffers[i]);
			VK_CHECK_RESULT(result);

			if (result != VK_SUCCESS)
			{
				NATIVE_ERROR("VulkanFramebuffer::Create: Failed to create framebuffer object!");
			}
		}

		m_ClearAttachments[0].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		m_ClearAttachments[0].clearValue.color = { { 0.1f, 0.1f, 0.1f, 1.0f} };
		m_ClearAttachments[0].colorAttachment = 0;

		m_ClearAttachments[1].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		m_ClearAttachments[1].clearValue.depthStencil = { 1.0f, 0 };

		return result;
	}

	void VulkanSwapchainFramebuffer::Clear()
	{
		for (auto& framebuffer : m_Framebuffers)
		{
			vkDestroyFramebuffer(m_Swapchain->m_Device->GetLogicalDevice(), framebuffer, nullptr);
		}
	}

	const std::vector<VkFramebuffer>& VulkanSwapchainFramebuffer::GetVkFramebuffers() const
	{
		return m_Framebuffers;
	}
}