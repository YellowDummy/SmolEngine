#include "stdafx.h"
#include "VulkanRenderPass.h"
#include "Renderer/Vulkan/VulkanContext.h"

namespace SmolEngine
{
    void VulkanRenderPass::Init()
    {
		// Offscreen
		{
			RenderPassCI info = {};

			CreateRenderPass(info, m_RenderPassFramebuffer);
		}

		// Swapchain
		{
			RenderPassCI info = {};
			info.ColorAttachmentFinalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			CreateRenderPass(info, m_RenderPassSwapchain);
		}

		// Deferred MSAA
		{
			RenderPassCI info = {};
			info.IsUseMRT = true;

			CreateRenderPass(info, m_MSAADeferredRenderPass);
		}
    }

    void VulkanRenderPass::Free()
    {
        VkDevice device = VulkanContext::GetDevice().GetLogicalDevice();

        if (m_MSAADeferredRenderPass != VK_NULL_HANDLE)
            vkDestroyRenderPass(device, m_MSAADeferredRenderPass, nullptr);

        if (m_RenderPassFramebuffer != VK_NULL_HANDLE)
            vkDestroyRenderPass(device, m_RenderPassFramebuffer, nullptr);

        if (m_RenderPassSwapchain != VK_NULL_HANDLE)
            vkDestroyRenderPass(device, m_RenderPassSwapchain, nullptr);
    }

    void VulkanRenderPass::CreateRenderPass(RenderPassCI& renderPassCI, VkRenderPass& renderPass)
    {
		VkDevice device = VulkanContext::GetDevice().GetLogicalDevice();
		VkFormat depthFormat = VulkanContext::GetSwapchain().GetDepthFormat();
		VkFormat colorFormat = VulkanContext::GetSwapchain().GetColorFormat();
        VkSampleCountFlagBits MSAASamplesCount = VulkanContext::GetDevice().GetMSAASamplesCount();
        std::vector<VkAttachmentDescription> attachments;

        if (renderPassCI.IsUseMRT)
        {
			attachments.resize(5);

			// Position
			attachments[0].format = VK_FORMAT_R16G16B16A16_SFLOAT;
			attachments[0].samples = MSAASamplesCount;
			attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachments[0].finalLayout = renderPassCI.ColorAttachmentFinalLayout;

			// Normal
			attachments[1].format = VK_FORMAT_R16G16B16A16_SFLOAT;
			attachments[1].samples = MSAASamplesCount;
			attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachments[1].finalLayout = renderPassCI.ColorAttachmentFinalLayout;

			// PBR: Metallic, Roughness, AO
			attachments[2].format = VK_FORMAT_R16G16B16A16_SFLOAT;
			attachments[2].samples = MSAASamplesCount;
			attachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachments[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachments[2].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[2].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachments[2].finalLayout = renderPassCI.ColorAttachmentFinalLayout;

			// Color
			attachments[3].format = colorFormat;
			attachments[3].samples = MSAASamplesCount;
			attachments[3].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachments[3].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachments[3].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[3].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[3].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachments[3].finalLayout = renderPassCI.ColorAttachmentFinalLayout;

			// Depth
			attachments[4].format = depthFormat;
			attachments[4].samples = MSAASamplesCount;
			attachments[4].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachments[4].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[4].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[4].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[4].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachments[4].finalLayout = renderPassCI.DepthAttachmentFinalLayout;
        }
        else
        {
			attachments.resize(2);

			attachments[0].format = colorFormat;
			attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
			attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachments[0].finalLayout = renderPassCI.ColorAttachmentFinalLayout;

			attachments[1].format = depthFormat;
			attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
			attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachments[1].finalLayout = renderPassCI.DepthAttachmentFinalLayout;
        }


		std::vector<VkAttachmentReference> colorReferences;
		if (renderPassCI.IsUseMRT)
		{
			colorReferences.resize(4);

			colorReferences[0].attachment = 0;
			colorReferences[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			colorReferences[1].attachment = 1;
			colorReferences[1].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			colorReferences[2].attachment = 2;
			colorReferences[2].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			colorReferences[3].attachment = 3;
			colorReferences[3].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		}
		else
		{
			colorReferences.resize(1);
			colorReferences[0].attachment = 0;
			colorReferences[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

		VkAttachmentReference depthReference = {};
		{
			depthReference.attachment = renderPassCI.IsUseMRT ? 4 : 1;
			depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}

		VkSubpassDescription subpass = {};
		{
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = static_cast<uint32_t>(colorReferences.size());
			subpass.pColorAttachments = colorReferences.data();
			subpass.pDepthStencilAttachment = &depthReference;
		}

		// Use subpass dependencies for attachment layout transitions
		std::array<VkSubpassDependency, 2> dependencies;
		{
			dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[0].dstSubpass = 0;
			dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			dependencies[1].srcSubpass = 0;
			dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		}

		VkRenderPassCreateInfo renderPassInfo = {};
		{
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			renderPassInfo.pAttachments = attachments.data();
			renderPassInfo.subpassCount = 1;
			renderPassInfo.pSubpasses = &subpass;
			renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
			renderPassInfo.pDependencies = dependencies.data();
		}

		VK_CHECK_RESULT(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass));
    }
}