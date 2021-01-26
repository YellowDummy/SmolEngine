#include "stdafx.h"
#include "VulkanFramebuffer.h"

#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanMemoryAllocator.h"
#include "Renderer/Vulkan/VulkanTexture.h"

#include "../Libraries/imgui/examples/imgui_impl_vulkan.h"

namespace SmolEngine
{
#define FB_COLOR_FORMAT VK_FORMAT_B8G8R8A8_UNORM

	VulkanFramebuffer::VulkanFramebuffer()
	{
		m_Device = VulkanContext::GetDevice().GetLogicalDevice();
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{

	}

	bool VulkanFramebuffer::Create(uint32_t width, uint32_t height)
	{
		VkResult result = VK_ERROR_UNKNOWN;
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
			vkGetPhysicalDeviceFormatProperties(VulkanContext::GetDevice().GetPhysicalDevice(), format, &formatProps);
			// Format must support depth stencil attachment for optimal tiling
			if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				fbDepthFormat = format;
				formatfound = true;
				break;
			}
		}
		assert(formatfound == true);

		// Color attachment
		{
			m_OffscreenPass.color.image = VulkanTexture::CreateVkImage(m_Specification.Width, m_Specification.Height,
				1,
				m_MSAASamples,
				FB_COLOR_FORMAT,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_OffscreenPass.color.mem);

			VkImageViewCreateInfo colorImageViewCI = {};
			{
				colorImageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				colorImageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
				colorImageViewCI.format = FB_COLOR_FORMAT;
				colorImageViewCI.components.r = VK_COMPONENT_SWIZZLE_R;
				colorImageViewCI.components.g = VK_COMPONENT_SWIZZLE_G;
				colorImageViewCI.components.b = VK_COMPONENT_SWIZZLE_B;
				colorImageViewCI.components.a = VK_COMPONENT_SWIZZLE_A;
				colorImageViewCI.subresourceRange = {};
				colorImageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				colorImageViewCI.subresourceRange.baseMipLevel = 0;
				colorImageViewCI.subresourceRange.baseArrayLayer = 0;
				colorImageViewCI.subresourceRange.levelCount = 1;
				colorImageViewCI.subresourceRange.layerCount = 1;
				colorImageViewCI.image = m_OffscreenPass.color.image;

				result = vkCreateImageView(m_Device, &colorImageViewCI, nullptr, &m_OffscreenPass.color.view);
				VK_CHECK_RESULT(result);
			}
		}

		// Resolve attachment
		{
			if (!m_Specification.IsTargetsSwapchain) // overwise swapchain image will be used as resolve attachment
			{
				m_OffscreenPass.resolve.image = VulkanTexture::CreateVkImage(m_Specification.Width, m_Specification.Height,
					1,
					VK_SAMPLE_COUNT_1_BIT,
					FB_COLOR_FORMAT,
					VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, m_OffscreenPass.resolve.mem);

				VkImageViewCreateInfo colorImageViewCI = {};
				{
					colorImageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
					colorImageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
					colorImageViewCI.format = FB_COLOR_FORMAT;
					colorImageViewCI.components.r = VK_COMPONENT_SWIZZLE_R;
					colorImageViewCI.components.g = VK_COMPONENT_SWIZZLE_G;
					colorImageViewCI.components.b = VK_COMPONENT_SWIZZLE_B;
					colorImageViewCI.components.a = VK_COMPONENT_SWIZZLE_A;
					colorImageViewCI.subresourceRange = {};
					colorImageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					colorImageViewCI.subresourceRange.baseMipLevel = 0;
					colorImageViewCI.subresourceRange.baseArrayLayer = 0;
					colorImageViewCI.subresourceRange.levelCount = 1;
					colorImageViewCI.subresourceRange.layerCount = 1;
					colorImageViewCI.image = m_OffscreenPass.resolve.image;

					result = vkCreateImageView(m_Device, &colorImageViewCI, nullptr, &m_OffscreenPass.resolve.view);
					VK_CHECK_RESULT(result);
				}
			}
		}

		// Depth stencil attachment
		{
			m_OffscreenPass.depth.image = VulkanTexture::CreateVkImage(m_Specification.Width, m_Specification.Height,
				1,
				m_MSAASamples,
				fbDepthFormat,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, m_OffscreenPass.depth.mem);

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
				depthStencilViewCI.components.r = VK_COMPONENT_SWIZZLE_R;
				depthStencilViewCI.components.g = VK_COMPONENT_SWIZZLE_G;
				depthStencilViewCI.components.b = VK_COMPONENT_SWIZZLE_B;
				depthStencilViewCI.components.a = VK_COMPONENT_SWIZZLE_A;

				result = vkCreateImageView(m_Device, &depthStencilViewCI, nullptr, &m_OffscreenPass.depth.view);
				VK_CHECK_RESULT(result);
			}
		}

		// Sampler
		{
			VkSamplerCreateInfo samplerCI = {};
			{
				auto& device = VulkanContext::GetDevice();

				samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
				samplerCI.magFilter = VK_FILTER_LINEAR;
				samplerCI.minFilter = VK_FILTER_LINEAR;
				samplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
				samplerCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
				samplerCI.addressModeV = samplerCI.addressModeU;
				samplerCI.addressModeW = samplerCI.addressModeU;
				samplerCI.mipLodBias = 0.0f;
				samplerCI.maxAnisotropy = 1.0f;
				samplerCI.minLod = 0.0f;
				samplerCI.maxLod = 1.0f;
				samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
				samplerCI.maxAnisotropy = 1.0f;
				if (device.GetDeviceFeatures()->samplerAnisotropy)
				{
					samplerCI.maxAnisotropy = device.GetDeviceProperties()->limits.maxSamplerAnisotropy;
					samplerCI.anisotropyEnable = VK_TRUE;
				}

				result = vkCreateSampler(m_Device, &samplerCI, nullptr, &m_OffscreenPass.sampler);
				VK_CHECK_RESULT(result);
			}
		}

		// Framebuffer creation
		{
			std::array<VkImageView, 3> attachments;

			attachments[0] = m_OffscreenPass.color.view;
			attachments[1] = m_OffscreenPass.resolve.view;
			attachments[2] = m_OffscreenPass.depth.view;

			m_OffscreenPass.clearAttachments[0].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			m_OffscreenPass.clearAttachments[0].clearValue.color  = { { 0.1f, 0.1f, 0.1f, 1.0f} };
			m_OffscreenPass.clearAttachments[0].colorAttachment = 0;

			m_OffscreenPass.clearAttachments[1].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			m_OffscreenPass.clearAttachments[1].clearValue.color = { { 0.1f, 0.1f, 0.1f, 1.0f} };
			m_OffscreenPass.clearAttachments[1].colorAttachment = 0;

			m_OffscreenPass.clearAttachments[2].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			m_OffscreenPass.clearAttachments[2].clearValue.depthStencil = { 1.0f, 0 };

			VkFramebufferCreateInfo fbufCreateInfo = {};
			{
				VkRenderPass renderPass = nullptr;
				m_Specification.IsTargetsSwapchain ? renderPass = renderPass = VulkanContext::GetVkRenderPassSwapchainLayout() :
					renderPass = VulkanContext::GetVkRenderPassFramebufferLayout();

				fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				fbufCreateInfo.pNext = NULL;
				fbufCreateInfo.renderPass = renderPass;
				fbufCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
				fbufCreateInfo.pAttachments = attachments.data();
				fbufCreateInfo.width = m_Specification.Width;
				fbufCreateInfo.height = m_Specification.Height;
				fbufCreateInfo.layers = 1;
			}

			if (!m_Specification.IsTargetsSwapchain)
			{
				m_VkFrameBuffers.resize(1);
				VK_CHECK_RESULT(vkCreateFramebuffer(m_Device, &fbufCreateInfo, nullptr, &m_VkFrameBuffers[0]));

				VkDescriptorImageInfo imageInfo = {};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = m_OffscreenPass.resolve.view;
				imageInfo.sampler = m_OffscreenPass.sampler;

				m_ImGuiTextureID = ImGui_ImplVulkan_AddTexture(imageInfo);
			}
			else
			{
				uint32_t count = VulkanContext::GetSwapchain().m_ImageCount;
				m_VkFrameBuffers.resize(count);
				for (uint32_t i = 0; i < count; ++i)
				{
					attachments[1] = VulkanContext::GetSwapchain().m_Buffers[i].View;
					VK_CHECK_RESULT(vkCreateFramebuffer(m_Device, &fbufCreateInfo, nullptr, &m_VkFrameBuffers[i]));
				}
			}
		}

		return result == VK_SUCCESS;
	}

	bool VulkanFramebuffer::Init(const FramebufferSpecification& data)
	{
		m_OffscreenPass = {};
		m_Specification = data;
		m_MSAASamples = VulkanContext::GetDevice().GetMSAASamplesCount();

		return Create(data.Width, data.Height);
	}

	void VulkanFramebuffer::OnResize(uint32_t width, uint32_t height)
	{
		FreeResources();
		Create(width, height);
	}

	void VulkanFramebuffer::FreeResources()
	{
		// Color attachment

		if (m_OffscreenPass.color.view != VK_NULL_HANDLE)
		{
			vkDestroyImageView(m_Device, m_OffscreenPass.color.view, nullptr);
		}
		if (m_OffscreenPass.color.image != VK_NULL_HANDLE)
		{
			vkDestroyImage(m_Device, m_OffscreenPass.color.image, nullptr);
		}
		if (m_OffscreenPass.color.mem != VK_NULL_HANDLE)
		{
			vkFreeMemory(m_Device, m_OffscreenPass.color.mem, nullptr);
		}

		// Resolve attachment

		if (m_OffscreenPass.resolve.view != VK_NULL_HANDLE)
		{
			vkDestroyImageView(m_Device, m_OffscreenPass.resolve.view, nullptr);
		}
		if (m_OffscreenPass.resolve.image != VK_NULL_HANDLE)
		{
			vkDestroyImage(m_Device, m_OffscreenPass.resolve.image, nullptr);
		}
		if (m_OffscreenPass.resolve.mem != VK_NULL_HANDLE)
		{
			vkFreeMemory(m_Device, m_OffscreenPass.resolve.mem, nullptr);
		}

		// Depth attachment

		if (m_OffscreenPass.depth.view != VK_NULL_HANDLE)
		{
			vkDestroyImageView(m_Device, m_OffscreenPass.depth.view, nullptr);
		}
		if (m_OffscreenPass.depth.image != VK_NULL_HANDLE)
		{
			vkDestroyImage(m_Device, m_OffscreenPass.depth.image, nullptr);
		}
		if (m_OffscreenPass.depth.mem != VK_NULL_HANDLE)
		{
			vkFreeMemory(m_Device, m_OffscreenPass.depth.mem, nullptr);
		}

		if (m_OffscreenPass.sampler != VK_NULL_HANDLE)
		{
			vkDestroySampler(m_Device, m_OffscreenPass.sampler, nullptr);
		}

		for (auto& fb : m_VkFrameBuffers)
		{
			if (fb != VK_NULL_HANDLE)
			{
				vkDestroyFramebuffer(m_Device, fb, nullptr);
			}
		}
	}

	void VulkanFramebuffer::SetClearColors(const glm::vec4& clearColors)
	{
		m_OffscreenPass.clearAttachments[0].clearValue = { { clearColors.r,  clearColors.g,  clearColors.b,  clearColors.a } };
		m_OffscreenPass.clearAttachments[1].clearValue = { { clearColors.r,  clearColors.g,  clearColors.b,  clearColors.a } };
	}

	const FramebufferSpecification& VulkanFramebuffer::GetSpecification() const
	{
		return m_Specification;
	}

	const VkFramebuffer VulkanFramebuffer::GetCurrentVkFramebuffer() const
	{
		uint32_t index;
		m_Specification.IsTargetsSwapchain ? index = VulkanContext::GetSwapchain().GetCurrentBufferIndex() :
			index = 0;
		return m_VkFrameBuffers[index];
	}

	const OffscreenPass& VulkanFramebuffer::GetOffscreenPass() const
	{
		return m_OffscreenPass;
	}

	void* VulkanFramebuffer::GetImGuiTextureID() const
	{
		return m_ImGuiTextureID;
	}
}