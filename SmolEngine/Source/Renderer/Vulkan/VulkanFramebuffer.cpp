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
			VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			AddAttachment(m_Specification.Width, m_Specification.Height, m_MSAASamples, usage, FB_COLOR_FORMAT,
				m_OffscreenPass.color.image, m_OffscreenPass.color.view, m_OffscreenPass.color.mem);
		}

		// Resolve attachment
		{
			if (!m_Specification.IsTargetsSwapchain) // overwise swapchain image will be used as resolve attachment
			{
				VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
				AddAttachment(m_Specification.Width, m_Specification.Height, VK_SAMPLE_COUNT_1_BIT, usage, FB_COLOR_FORMAT,
					m_OffscreenPass.resolve.image, m_OffscreenPass.resolve.view, m_OffscreenPass.resolve.mem);
			}
		}

		// Depth stencil attachment
		{
			VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			AddAttachment(m_Specification.Width, m_Specification.Height, m_MSAASamples, usage, fbDepthFormat,
				m_OffscreenPass.depth.image, m_OffscreenPass.depth.view, m_OffscreenPass.depth.mem, imageAspect);
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

				result = vkCreateSampler(m_Device, &samplerCI, nullptr, &m_Sampler);
				VK_CHECK_RESULT(result);
			}
		}

		// Framebuffer creation
		{
			std::vector<VkImageView> attachments;

			if (m_Specification.IsUseMRT)
			{
				
			}
			else
			{
				attachments.resize(3);
				attachments[0] = m_OffscreenPass.color.view;
				attachments[1] = m_OffscreenPass.resolve.view;
				attachments[2] = m_OffscreenPass.depth.view;

				m_OffscreenPass.clearAttachments[0].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				m_OffscreenPass.clearAttachments[0].clearValue.color = { { 0.1f, 0.1f, 0.1f, 1.0f} };
				m_OffscreenPass.clearAttachments[0].colorAttachment = 0;

				m_OffscreenPass.clearAttachments[1].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				m_OffscreenPass.clearAttachments[1].clearValue.color = { { 0.1f, 0.1f, 0.1f, 1.0f} };
				m_OffscreenPass.clearAttachments[1].colorAttachment = 0;

				m_OffscreenPass.clearAttachments[2].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
				m_OffscreenPass.clearAttachments[2].clearValue.depthStencil = { 1.0f, 0 };
			}

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
				imageInfo.sampler = m_Sampler;

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

	bool VulkanFramebuffer::CreateDeferred()
	{
#define FB_DIM 2048
		VkFormat fbDepthFormat;
		{
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

			if (!formatfound)
			{
				return false;
				assert(formatfound == true);
			}
		}

		// Position
		{
			VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			AddAttachment(FB_DIM, FB_DIM, m_MSAASamples, usage, VK_FORMAT_R16G16B16A16_SFLOAT,
				m_DeferredPass.position.image, m_DeferredPass.position.view, m_DeferredPass.position.mem);
		}

		// Normals
		{
			VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			AddAttachment(FB_DIM, FB_DIM, m_MSAASamples, usage, VK_FORMAT_R16G16B16A16_SFLOAT,
				m_DeferredPass.normals.image, m_DeferredPass.normals.view, m_DeferredPass.normals.mem);
		}

		// Albedo (color)
		{
			VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			AddAttachment(FB_DIM, FB_DIM, m_MSAASamples, usage, FB_COLOR_FORMAT,
				m_DeferredPass.color.image, m_DeferredPass.color.view, m_DeferredPass.color.mem);
		}

		// Depth 
		{
			VkImageUsageFlags usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			AddAttachment(FB_DIM, FB_DIM, m_MSAASamples, usage, fbDepthFormat,
				m_DeferredPass.depth.image, m_DeferredPass.depth.view, m_DeferredPass.depth.mem, imageAspect);
		}

		// Sampler
		{
			VkSamplerCreateInfo samplerCI = {};
			{
				auto& device = VulkanContext::GetDevice();

				samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
				samplerCI.magFilter = VK_FILTER_NEAREST;
				samplerCI.minFilter = VK_FILTER_NEAREST;
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

				VK_CHECK_RESULT(vkCreateSampler(m_Device, &samplerCI, nullptr, &m_Sampler));
			}
		}


		//Position
		m_DeferredPass.positionImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		m_DeferredPass.positionImageInfo.imageView = m_DeferredPass.position.view;
		m_DeferredPass.positionImageInfo.sampler = m_Sampler;

		//Normals
		m_DeferredPass.normalsImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		m_DeferredPass.normalsImageInfo.imageView = m_DeferredPass.normals.view;
		m_DeferredPass.normalsImageInfo.sampler = m_Sampler;

		//Albedo (color)
		m_DeferredPass.colorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		m_DeferredPass.colorImageInfo.imageView = m_DeferredPass.color.view;
		m_DeferredPass.colorImageInfo.sampler = m_Sampler;

		return true;
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

		if (m_Sampler != VK_NULL_HANDLE)
		{
			vkDestroySampler(m_Device, m_Sampler, nullptr);
		}

		for (auto& fb : m_VkFrameBuffers)
		{
			if (fb != VK_NULL_HANDLE)
			{
				vkDestroyFramebuffer(m_Device, fb, nullptr);
			}
		}
	}

	void VulkanFramebuffer::AddAttachment(uint32_t width, uint32_t height,
		VkSampleCountFlagBits samples, VkImageUsageFlags imageUsage, VkFormat format,
		VkImage& image, VkImageView& imageView, VkDeviceMemory& mem, VkImageAspectFlags imageAspect)
	{
		image = VulkanTexture::CreateVkImage(width, height,
			1,
			samples,
			format,
			VK_IMAGE_TILING_OPTIMAL,
			imageUsage, mem);

		VkImageViewCreateInfo colorImageViewCI = {};
		{
			colorImageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			colorImageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
			colorImageViewCI.format = format;
			colorImageViewCI.components.r = VK_COMPONENT_SWIZZLE_R;
			colorImageViewCI.components.g = VK_COMPONENT_SWIZZLE_G;
			colorImageViewCI.components.b = VK_COMPONENT_SWIZZLE_B;
			colorImageViewCI.components.a = VK_COMPONENT_SWIZZLE_A;
			colorImageViewCI.subresourceRange = {};
			colorImageViewCI.subresourceRange.aspectMask = imageAspect;
			colorImageViewCI.subresourceRange.baseMipLevel = 0;
			colorImageViewCI.subresourceRange.baseArrayLayer = 0;
			colorImageViewCI.subresourceRange.levelCount = 1;
			colorImageViewCI.subresourceRange.layerCount = 1;
			colorImageViewCI.image = image;

			VK_CHECK_RESULT(vkCreateImageView(m_Device, &colorImageViewCI, nullptr, &imageView));
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