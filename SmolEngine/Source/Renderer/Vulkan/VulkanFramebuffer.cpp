#include "stdafx.h"
#include "VulkanFramebuffer.h"

#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanRenderPass.h"
#include "Renderer/Vulkan/VulkanMemoryAllocator.h"
#include "Renderer/Vulkan/VulkanTexture.h"
#include "Renderer/Vulkan/VulkanSemaphore.h"

#include "../Libraries/imgui/examples/imgui_impl_vulkan.h"

namespace SmolEngine
{
	VulkanFramebuffer::VulkanFramebuffer()
	{
		m_Device = VulkanContext::GetDevice().GetLogicalDevice();
		m_ColorFormat = VulkanContext::GetSwapchain().GetColorFormat();
		m_DepthFormat = VulkanContext::GetSwapchain().GetDepthFormat();
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		//FreeResources();
	}

	bool VulkanFramebuffer::Create(uint32_t width, uint32_t height)
	{
		VkResult result = VK_ERROR_UNKNOWN;
		m_Specification.Width = width;
		m_Specification.Height = height;

		// Color attachment
		{
			VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			AddAttachment(m_Specification.Width, m_Specification.Height, m_MSAASamples, usage, m_ColorFormat,
				m_OffscreenPass.color.image, m_OffscreenPass.color.view, m_OffscreenPass.color.mem);
		}

		// Resolve attachment
		{
			if (!m_Specification.IsTargetsSwapchain)
			{
				VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
				AddAttachment(m_Specification.Width, m_Specification.Height, VK_SAMPLE_COUNT_1_BIT, usage, m_ColorFormat,
					m_OffscreenPass.color.image, m_OffscreenPass.color.view, m_OffscreenPass.color.mem);
			}
		}

		// Depth stencil attachment
		{
			VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			AddAttachment(m_Specification.Width, m_Specification.Height, m_MSAASamples, usage, m_DepthFormat,
				m_OffscreenPass.depth.image, m_OffscreenPass.depth.view, m_OffscreenPass.depth.mem, imageAspect);
		}

		// Sampler
		CreateSampler();

		// Framebuffer creation
		{
			std::vector<VkImageView> attachments(3);
			{
				attachments[0] = m_OffscreenPass.color.view;
				attachments[1] = m_OffscreenPass.resolve.view;
				attachments[2] = m_OffscreenPass.depth.view;
			}

			VkFramebufferCreateInfo fbufCreateInfo = {};
			{
				VkRenderPass renderPass = nullptr;
				m_Specification.IsTargetsSwapchain ? renderPass = renderPass = VulkanRenderPass::GetVkRenderPassSwapchainLayout() :
					renderPass = VulkanRenderPass::GetVkRenderPassFramebufferLayout();

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

		// Clear
		{
			if (m_ClearAttachments.size() > 0)
				m_ClearAttachments.clear();

			m_ClearAttachments.resize(3);

			m_ClearAttachments[0].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			m_ClearAttachments[0].clearValue.color = { { 0.1f, 0.1f, 0.1f, 1.0f} };
			m_ClearAttachments[0].colorAttachment = 0;

			m_ClearAttachments[1].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			m_ClearAttachments[1].clearValue.color = { { 0.1f, 0.1f, 0.1f, 1.0f} };
			m_ClearAttachments[1].colorAttachment = 0;

			m_ClearAttachments[2].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			m_ClearAttachments[2].clearValue.depthStencil = { 1.0f, 0 };
		}

		return result == VK_SUCCESS;
	}

	bool VulkanFramebuffer::CreateDeferred(uint32_t width, uint32_t height)
	{
		// Position
		{
			VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			AddAttachment(width, height, m_MSAASamples, usage, VK_FORMAT_R16G16B16A16_SFLOAT,
				m_DeferredPass.position.image, m_DeferredPass.position.view, m_DeferredPass.position.mem);
		}

		// Normals
		{
			VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			AddAttachment(width, height, m_MSAASamples, usage, VK_FORMAT_R16G16B16A16_SFLOAT,
				m_DeferredPass.normals.image, m_DeferredPass.normals.view, m_DeferredPass.normals.mem);
		}

		// PBR
		{
			VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			AddAttachment(width, height, m_MSAASamples, usage, VK_FORMAT_R16G16B16A16_SFLOAT,
				m_DeferredPass.pbr.image, m_DeferredPass.pbr.view, m_DeferredPass.pbr.mem);
		}

		// Albedo (color)
		{
			VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			AddAttachment(width, height, m_MSAASamples, usage, m_ColorFormat,
				m_DeferredPass.color.image, m_DeferredPass.color.view, m_DeferredPass.color.mem);
		}

		// Depth 
		{
			VkImageUsageFlags usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			AddAttachment(width, height, m_MSAASamples, usage, m_DepthFormat,
				m_DeferredPass.depth.image, m_DeferredPass.depth.view, m_DeferredPass.depth.mem, imageAspect);
		}

		// Sampler
		CreateSampler();

		// Framebuffer 
		{
			std::vector<VkImageView> attachments(5);
			{
				attachments[0] = m_DeferredPass.position.view;
				attachments[1] = m_DeferredPass.normals.view;
				attachments[2] = m_DeferredPass.pbr.view;
				attachments[3] = m_DeferredPass.color.view;
				attachments[4] = m_DeferredPass.depth.view;
			}

			VkFramebufferCreateInfo fbufCreateInfo = {};
			{
				fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				fbufCreateInfo.pNext = NULL;
				fbufCreateInfo.renderPass = VulkanRenderPass::GetVkRenderPassDeferredLayout();
				fbufCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
				fbufCreateInfo.pAttachments = attachments.data();
				fbufCreateInfo.width = width;
				fbufCreateInfo.height = height;
				fbufCreateInfo.layers = 1;
			}

			m_VkFrameBuffers.resize(1);
			VK_CHECK_RESULT(vkCreateFramebuffer(m_Device, &fbufCreateInfo, nullptr, &m_VkFrameBuffers[0]));
		}

		// Clear
		{
			if (m_ClearAttachments.size() > 0)
				m_ClearAttachments.clear();

			m_ClearAttachments.resize(5);

			m_ClearAttachments[0].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			m_ClearAttachments[0].clearValue.color = { { 0.1f, 0.1f, 0.1f, 1.0f} };
			m_ClearAttachments[0].colorAttachment = 0;

			m_ClearAttachments[1].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			m_ClearAttachments[1].clearValue.color = { { 0.1f, 0.1f, 0.1f, 1.0f} };
			m_ClearAttachments[1].colorAttachment = 1;

			m_ClearAttachments[2].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			m_ClearAttachments[2].clearValue.color = { { 0.1f, 0.1f, 0.1f, 1.0f} };
			m_ClearAttachments[2].colorAttachment = 2;

			m_ClearAttachments[3].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			m_ClearAttachments[3].clearValue.color = { { 0.1f, 0.1f, 0.1f, 1.0f} };
			m_ClearAttachments[3].colorAttachment = 3;

			m_ClearAttachments[4].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			m_ClearAttachments[4].clearValue.depthStencil = { 1.0f, 0 };
		}

		// Create a semaphore used to synchronize offscreen rendering and usage
		VulkanSemaphore::CreateVkSemaphore(m_DeferredPass.semaphore);

		// DescriptorImageInfo
		{
			//Position
			m_DeferredPass.positionImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			m_DeferredPass.positionImageInfo.imageView = m_DeferredPass.position.view;
			m_DeferredPass.positionImageInfo.sampler = m_Sampler;

			//Normals
			m_DeferredPass.normalsImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			m_DeferredPass.normalsImageInfo.imageView = m_DeferredPass.normals.view;
			m_DeferredPass.normalsImageInfo.sampler = m_Sampler;

			//PBR
			m_DeferredPass.pbrImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			m_DeferredPass.pbrImageInfo.imageView = m_DeferredPass.pbr.view;
			m_DeferredPass.pbrImageInfo.sampler = m_Sampler;

			//Albedo (color)
			m_DeferredPass.colorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			m_DeferredPass.colorImageInfo.imageView = m_DeferredPass.color.view;
			m_DeferredPass.colorImageInfo.sampler = m_Sampler;
		}

		m_Specification.Width = width;
		m_Specification.Height = height;

		return true;
	}

	void VulkanFramebuffer::CreateSampler()
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

	bool VulkanFramebuffer::Init(const FramebufferSpecification& data)
	{
		m_OffscreenPass = {};
		m_Specification = data;
		m_MSAASamples = VulkanContext::GetDevice().GetMSAASamplesCount();

		bool result = false;
		data.IsUseMRT ? result = CreateDeferred(m_DeferredDim, m_DeferredDim) : result = Create(data.Width, data.Height);
		return result;
	}

	void VulkanFramebuffer::OnResize(uint32_t width, uint32_t height)
	{
		if (!m_Specification.IsUseMRT)
		{
			FreeResources();
			Create(width, height);
		}
	}

	void VulkanFramebuffer::FreeResources()
	{
		// Offscreen
		{
			FreeAttachment(m_OffscreenPass.color);
			FreeAttachment(m_OffscreenPass.depth);
			FreeAttachment(m_OffscreenPass.resolve);
		}

		// Deferred
		{
			FreeAttachment(m_DeferredPass.position);
			FreeAttachment(m_DeferredPass.normals);
			FreeAttachment(m_DeferredPass.pbr);
			FreeAttachment(m_DeferredPass.color);
			FreeAttachment(m_DeferredPass.depth);
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

	void VulkanFramebuffer::FreeAttachment(FrameBufferAttachment& framebuffer)
	{
		if (framebuffer.view != VK_NULL_HANDLE)
			vkDestroyImageView(m_Device, framebuffer.view, nullptr);

		if (framebuffer.image != VK_NULL_HANDLE)
			vkDestroyImage(m_Device, framebuffer.image, nullptr);

		if (framebuffer.mem != VK_NULL_HANDLE)
			vkFreeMemory(m_Device, framebuffer.mem, nullptr);
	}

	void VulkanFramebuffer::SetClearColors(const glm::vec4& clearColors)
	{
		if (m_Specification.IsUseMRT)
		{
			m_ClearAttachments[0].clearValue = { { clearColors.r,  clearColors.g,  clearColors.b,  clearColors.a } };
			m_ClearAttachments[1].clearValue = { { clearColors.r,  clearColors.g,  clearColors.b,  clearColors.a } };
			m_ClearAttachments[2].clearValue = { { clearColors.r,  clearColors.g,  clearColors.b,  clearColors.a } };
			m_ClearAttachments[3].clearValue = { { clearColors.r,  clearColors.g,  clearColors.b,  clearColors.a } };
		}
		else
		{
			m_ClearAttachments[0].clearValue = { { clearColors.r,  clearColors.g,  clearColors.b,  clearColors.a } };
			m_ClearAttachments[1].clearValue = { { clearColors.r,  clearColors.g,  clearColors.b,  clearColors.a } };
		}
	}

	const std::vector<VkClearAttachment>& VulkanFramebuffer::GetClearAttachments() const
	{
		return m_ClearAttachments;
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

	const DeferredPass& VulkanFramebuffer::GetDeferredPass() const
	{
		return m_DeferredPass;
	}
}