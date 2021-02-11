#include "stdafx.h"
#include "VulkanRenderPass.h"
#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanFramebuffer.h"

namespace SmolEngine
{
	void VulkanRenderPass::Create(FramebufferSpecification* framebufferSpec, RenderPassGenInfo* renderPassInfo, VkRenderPass& outPass)
	{
		VkDevice device = VulkanContext::GetDevice().GetLogicalDevice();

		std::vector<VkAttachmentReference> colorReferences(renderPassInfo->NumColorAttachments);
		std::vector<VkAttachmentReference> depthReferences(renderPassInfo->NumDepthAttachments);
		std::vector<VkAttachmentReference> resolveReferences(renderPassInfo->NumResolveAttachments);

		std::vector<VkAttachmentDescription> attachments(renderPassInfo->NumColorAttachments + renderPassInfo->NumDepthAttachments
			+ renderPassInfo->NumResolveAttachments);

		uint32_t lastAttachmentIndex = 0;
		for (uint32_t i = 0; i < renderPassInfo->NumColorAttachments; ++i)
		{
			VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			if (!framebufferSpec->Attachments[i].bClearOp)
				loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

			VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			if (framebufferSpec->bTargetsSwapchain)
				layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			// attachment
			attachments[lastAttachmentIndex].format = VulkanFramebuffer::GetAttachmentFormat(framebufferSpec->Attachments[i].Format);
			attachments[lastAttachmentIndex].samples = renderPassInfo->MSAASamples;
			attachments[lastAttachmentIndex].loadOp = loadOp;
			attachments[lastAttachmentIndex].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachments[lastAttachmentIndex].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[lastAttachmentIndex].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[lastAttachmentIndex].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachments[lastAttachmentIndex].finalLayout = layout;

			// referense
			colorReferences[i].attachment = lastAttachmentIndex;
			colorReferences[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			lastAttachmentIndex++;
		}


		for (uint32_t i = 0; i < renderPassInfo->NumResolveAttachments; ++i)
		{
			VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			if (framebufferSpec->bTargetsSwapchain)
				layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			if (!framebufferSpec->Attachments[i].bClearOp)
				loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

			attachments[lastAttachmentIndex].format = renderPassInfo->ColorFormat;
			attachments[lastAttachmentIndex].samples = VK_SAMPLE_COUNT_1_BIT;
			attachments[lastAttachmentIndex].loadOp = loadOp;
			attachments[lastAttachmentIndex].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachments[lastAttachmentIndex].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
			attachments[lastAttachmentIndex].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[lastAttachmentIndex].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachments[lastAttachmentIndex].finalLayout = layout;

			resolveReferences[i].attachment = lastAttachmentIndex;
			resolveReferences[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			lastAttachmentIndex++;
		}

		for (uint32_t i = 0; i < renderPassInfo->NumDepthAttachments; ++i)
		{
			attachments[lastAttachmentIndex].format = renderPassInfo->DepthFormat;
			attachments[lastAttachmentIndex].samples = renderPassInfo->MSAASamples;
			attachments[lastAttachmentIndex].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachments[lastAttachmentIndex].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[lastAttachmentIndex].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[lastAttachmentIndex].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[lastAttachmentIndex].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachments[lastAttachmentIndex].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

			depthReferences[i].attachment = lastAttachmentIndex;
			depthReferences[i].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			lastAttachmentIndex++;
		}

		VkSubpassDescription subpass = {};
		{
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = static_cast<uint32_t>(colorReferences.size());
			subpass.pColorAttachments = colorReferences.data();
			subpass.pDepthStencilAttachment = depthReferences.data();

			if (framebufferSpec->bUseMSAA)
				subpass.pResolveAttachments = resolveReferences.data();
		}

		std::vector<VkSubpassDependency> dependencies(framebufferSpec->NumDependencies * 2);
		{
			uint32_t subPassIndex = 0;
			for (uint32_t i = 0; i < framebufferSpec->NumDependencies * 2; ++i)
			{
				dependencies[i].srcSubpass = VK_SUBPASS_EXTERNAL;
				dependencies[i].dstSubpass = subPassIndex;
				dependencies[i].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
				dependencies[i].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependencies[i].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
				dependencies[i].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

				i++;

				dependencies[i].srcSubpass = subPassIndex;
				dependencies[i].dstSubpass = VK_SUBPASS_EXTERNAL;
				dependencies[i].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependencies[i].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
				dependencies[i].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependencies[i].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
				dependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

				subPassIndex++;
			}
		}

		VkRenderPassCreateInfo renderPassCI = {};
		{
			renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassCI.attachmentCount = static_cast<uint32_t>(attachments.size());
			renderPassCI.pAttachments = attachments.data();
			renderPassCI.subpassCount = 1;
			renderPassCI.pSubpasses = &subpass;
			renderPassCI.dependencyCount = static_cast<uint32_t>(dependencies.size());
			renderPassCI.pDependencies = dependencies.data();
		}

		VK_CHECK_RESULT(vkCreateRenderPass(device, &renderPassCI, nullptr, &outPass));
	}
}