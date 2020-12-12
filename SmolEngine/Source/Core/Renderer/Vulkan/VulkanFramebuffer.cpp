#include "stdafx.h"
#include "VulkanFramebuffer.h"

#include "Core/Renderer/Vulkan/VulkanContext.h"
#include "Core/Renderer/Vulkan/VulkanMemoryAllocator.h"

#include "../Libraries/imgui/examples/imgui_impl_vulkan.h"

namespace SmolEngine
{
#define FB_COLOR_FORMAT VK_FORMAT_B8G8R8A8_UNORM

	VulkanFramebuffer::VulkanFramebuffer()
	{

	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{

	}

	bool VulkanFramebuffer::Create(uint32_t width, uint32_t height)
	{
		VkResult result = VK_ERROR_UNKNOWN;
		const auto& device = *VulkanContext::GetDevice().GetLogicalDevice();

		m_Specification.Width = width;
		m_Specification.Height = height;

		VkFormat fbDepthFormat;
		std::vector<VkFormat> depthFormats = {
				VK_FORMAT_D32_SFLOAT_S8_UINT,
				VK_FORMAT_D32_SFLOAT,
				VK_FORMAT_D24_UNORM_S8_UINT,
				VK_FORMAT_D16_UNORM_S8_UINT,
				VK_FORMAT_D16_UNORM
		};

		bool formatfound = false;
		for (auto& format : depthFormats)
		{
			VkFormatProperties formatProps;
			vkGetPhysicalDeviceFormatProperties(*VulkanContext::GetDevice().GetPhysicalDevice(), format, &formatProps);
			// Format must support depth stencil attachment for optimal tiling
			if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				fbDepthFormat = format;
				formatfound = true;
				break;
			}
		}
		assert(formatfound == true);

		VkMemoryRequirements memReqs;
		VkImageCreateInfo imageCI = {};
		{
			imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCI.imageType = VK_IMAGE_TYPE_2D;
			imageCI.format = FB_COLOR_FORMAT;
			imageCI.extent.width = m_Specification.Width;
			imageCI.extent.height = m_Specification.Height;
			imageCI.extent.depth = 1;
			imageCI.mipLevels = 1;
			imageCI.arrayLayers = 1;
			imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
			imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
			// We will sample directly from the color attachment
			imageCI.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

			result = vkCreateImage(device, &imageCI, nullptr, &m_OffscreenPass.color.image);
			VK_CHECK_RESULT(result);
			vkGetImageMemoryRequirements(device, m_OffscreenPass.color.image, &memReqs);
			VulkanMemoryAllocator::Allocate(device, memReqs, &m_OffscreenPass.color.mem, VulkanContext::GetDevice().GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
			result = vkBindImageMemory(device, m_OffscreenPass.color.image, m_OffscreenPass.color.mem, 0);
			VK_CHECK_RESULT(result);
		}

		VkImageViewCreateInfo colorImageViewCI = {};
		{
			colorImageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			colorImageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
			colorImageViewCI.format = FB_COLOR_FORMAT;
			colorImageViewCI.subresourceRange = {};
			colorImageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			colorImageViewCI.subresourceRange.baseMipLevel = 0;
			colorImageViewCI.subresourceRange.levelCount = 1;
			colorImageViewCI.subresourceRange.baseArrayLayer = 0;
			colorImageViewCI.subresourceRange.layerCount = 1;
			colorImageViewCI.image = m_OffscreenPass.color.image;

			result = vkCreateImageView(device, &colorImageViewCI, nullptr, &m_OffscreenPass.color.view);
			VK_CHECK_RESULT(result);
		}

		VkSamplerCreateInfo samplerInfoCI = {};
		{
			samplerInfoCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfoCI.magFilter = VK_FILTER_LINEAR;
			samplerInfoCI.minFilter = VK_FILTER_LINEAR;
			samplerInfoCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerInfoCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerInfoCI.addressModeV = samplerInfoCI.addressModeU;
			samplerInfoCI.addressModeW = samplerInfoCI.addressModeU;
			samplerInfoCI.mipLodBias = 0.0f;
			samplerInfoCI.maxAnisotropy = 1.0f;
			samplerInfoCI.minLod = 0.0f;
			samplerInfoCI.maxLod = 1.0f;
			samplerInfoCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

			result = vkCreateSampler(device, &samplerInfoCI, nullptr, &m_OffscreenPass.sampler);
			VK_CHECK_RESULT(result);
		}

		// Depth stencil attachment
		memReqs = {};
		imageCI.format = fbDepthFormat;
		imageCI.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

		result = vkCreateImage(device, &imageCI, nullptr, &m_OffscreenPass.depth.image);
		VK_CHECK_RESULT(result);
		vkGetImageMemoryRequirements(device, m_OffscreenPass.depth.image, &memReqs);
		VulkanMemoryAllocator::Allocate(device, memReqs, &m_OffscreenPass.depth.mem, VulkanContext::GetDevice().GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
		result = vkBindImageMemory(device, m_OffscreenPass.depth.image, m_OffscreenPass.depth.mem, 0);
		VK_CHECK_RESULT(result);

		VkImageViewCreateInfo depthStencilViewCI = {};
		{
			depthStencilViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			depthStencilViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
			depthStencilViewCI.format = fbDepthFormat;
			depthStencilViewCI.flags = 0;
			depthStencilViewCI.subresourceRange = {};
			depthStencilViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			depthStencilViewCI.subresourceRange.baseMipLevel = 0;
			depthStencilViewCI.subresourceRange.levelCount = 1;
			depthStencilViewCI.subresourceRange.baseArrayLayer = 0;
			depthStencilViewCI.subresourceRange.layerCount = 1;
			depthStencilViewCI.image = m_OffscreenPass.depth.image;

			result = vkCreateImageView(device, &depthStencilViewCI, nullptr, &m_OffscreenPass.depth.view);
			VK_CHECK_RESULT(result);
		}

		{
			std::array<VkAttachmentDescription, 2> attchmentDescriptions = {};

			// Color attachment
			attchmentDescriptions[0].format = FB_COLOR_FORMAT;
			attchmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
			attchmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attchmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attchmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attchmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attchmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attchmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			// Depth attachment
			attchmentDescriptions[1].format = fbDepthFormat;
			attchmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
			attchmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attchmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attchmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attchmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attchmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attchmentDescriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			VkAttachmentReference colorReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
			VkAttachmentReference depthReference = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

			VkSubpassDescription subpassDescription = {};
			subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescription.colorAttachmentCount = 1;
			subpassDescription.pColorAttachments = &colorReference;
			subpassDescription.pDepthStencilAttachment = &depthReference;

			// Use subpass dependencies for layout transitions
			std::array<VkSubpassDependency, 2> dependencies;

			dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[0].dstSubpass = 0;
			dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			dependencies[1].srcSubpass = 0;
			dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			// Create the actual renderpass
			VkRenderPassCreateInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = static_cast<uint32_t>(attchmentDescriptions.size());
			renderPassInfo.pAttachments = attchmentDescriptions.data();
			renderPassInfo.subpassCount = 1;
			renderPassInfo.pSubpasses = &subpassDescription;
			renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
			renderPassInfo.pDependencies = dependencies.data();

			result = vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_OffscreenPass.renderPass);
			VK_CHECK_RESULT(result);

			VkImageView attachments[2];
			attachments[0] = m_OffscreenPass.color.view;
			attachments[1] = m_OffscreenPass.depth.view;

			VkFramebufferCreateInfo fbufCreateInfo = {};
			{
				fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				fbufCreateInfo.renderPass = m_OffscreenPass.renderPass;
				fbufCreateInfo.attachmentCount = 2;
				fbufCreateInfo.pAttachments = attachments;
				fbufCreateInfo.width = m_Specification.Width;
				fbufCreateInfo.height = m_Specification.Height;
				fbufCreateInfo.layers = 1;

				result = vkCreateFramebuffer(device, &fbufCreateInfo, nullptr, &m_OffscreenPass.frameBuffer);
				VK_CHECK_RESULT(result);
			}

			// Fill a descriptor for later use in a descriptor set
			m_OffscreenPass.descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			m_OffscreenPass.descriptor.imageView = m_OffscreenPass.color.view;
			m_OffscreenPass.descriptor.sampler = m_OffscreenPass.sampler;

			m_OffscreenPass.ImGuiTextureID = ImGui_ImplVulkan_AddTexture(m_OffscreenPass.descriptor);
		}

		return result == VK_SUCCESS;
	}

	bool VulkanFramebuffer::Init(const FramebufferSpecification& data)
	{
		m_OffscreenPass = {};
		return Create(data.Width, data.Height);
	}

	void VulkanFramebuffer::OnResize(uint32_t width, uint32_t height)
	{
		Create(width, height);
	}

	void VulkanFramebuffer::FreeResources()
	{
		const auto& device = *VulkanContext::GetDevice().GetLogicalDevice();

		// Color attachment

		if (m_OffscreenPass.color.view != VK_NULL_HANDLE)
		{
			vkDestroyImageView(device, m_OffscreenPass.color.view, nullptr);
		}
		if (m_OffscreenPass.color.image != VK_NULL_HANDLE)
		{
			vkDestroyImage(device, m_OffscreenPass.color.image, nullptr);
		}
		if (m_OffscreenPass.color.mem != VK_NULL_HANDLE)
		{
			vkFreeMemory(device, m_OffscreenPass.color.mem, nullptr);
		}

		// Depth attachment

		if (m_OffscreenPass.depth.view != VK_NULL_HANDLE)
		{
			vkDestroyImageView(device, m_OffscreenPass.depth.view, nullptr);
		}
		if (m_OffscreenPass.depth.image != VK_NULL_HANDLE)
		{
			vkDestroyImage(device, m_OffscreenPass.depth.image, nullptr);
		}
		if (m_OffscreenPass.depth.mem != VK_NULL_HANDLE)
		{
			vkFreeMemory(device, m_OffscreenPass.depth.mem, nullptr);
		}

		if (m_OffscreenPass.renderPass != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(device, m_OffscreenPass.renderPass, nullptr);
		}

		if (m_OffscreenPass.sampler != VK_NULL_HANDLE)
		{
			vkDestroySampler(device, m_OffscreenPass.sampler, nullptr);
		}

		if (m_OffscreenPass.frameBuffer != VK_NULL_HANDLE)
		{
			vkDestroyFramebuffer(device, m_OffscreenPass.frameBuffer, nullptr);
		}
	}

	const FramebufferSpecification& VulkanFramebuffer::GetSpecification() const
	{
		return m_Specification;
	}

	const OffscreenPass& VulkanFramebuffer::GetOffscreenPass() const
	{
		return m_OffscreenPass;
	}

	void* VulkanFramebuffer::GetImGuiTextureID() const
	{
		return m_OffscreenPass.ImGuiTextureID;
	}
}