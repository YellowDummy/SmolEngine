#include "stdafx.h"
#include "VulkanFramebuffer.h"

#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanRenderPass.h"
#include "Renderer/Vulkan/VulkanMemoryAllocator.h"
#include "Renderer/Vulkan/VulkanTexture.h"
#include "Renderer/Vulkan/VulkanSemaphore.h"
#include "Renderer/Framebuffer.h"

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
		FreeResources();

		if (m_RenderPass != VK_NULL_HANDLE)
			vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
	}

	bool VulkanFramebuffer::Create(uint32_t width, uint32_t height)
	{
		if (m_Specification.Attachments.size() > 1 && m_Specification.bTargetsSwapchain || m_Specification.Attachments.size() == 0
			|| m_Specification.Attachments.size() > 1 && m_Specification.bUseMSAA)
			return false;

		m_Specification.Width = width;
		m_Specification.Height = height;

		uint32_t lastImageViewIndex = 0;
		uint32_t bufferSize = static_cast<uint32_t>(m_Specification.Attachments.size());
		uint32_t attachmentsCount = m_Specification.bUseMSAA ? bufferSize + 2 : bufferSize + 1;

		m_Attachments.resize(bufferSize);
		m_ClearValues.resize(attachmentsCount);
		m_ClearAttachments.resize(attachmentsCount);
		std::vector<VkImageView> attachments(attachmentsCount);

		// Sampler
		CreateSampler();

		// Color Attachments
		for (uint32_t i = 0; i < bufferSize; ++i)
		{
			auto& info = m_Specification.Attachments[i];
			auto& vkInfo = m_Attachments[i];

			VkImageUsageFlags usage = m_Specification.bUseMSAA ? VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT :
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

			AddAttachment(width, height, m_MSAASamples, usage, GetAttachmentFormat(info.Format),
				vkInfo.AttachmentVkInfo.image, vkInfo.AttachmentVkInfo.view, vkInfo.AttachmentVkInfo.mem);

			if (!m_Specification.bUseMSAA)
			{
				vkInfo.ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				vkInfo.ImageInfo.imageView = vkInfo.AttachmentVkInfo.view;
				vkInfo.ImageInfo.sampler = m_Sampler;

				if (m_Specification.bUsedByImGui)
				{
					vkInfo.ImGuiID = ImGui_ImplVulkan_AddTexture(vkInfo.ImageInfo);
				}
			}

			attachments[lastImageViewIndex] = vkInfo.AttachmentVkInfo.view;

			m_ClearValues[lastImageViewIndex].color = { { 0.0f, 0.0f, 0.0f, 1.0f} };
			m_ClearAttachments[lastImageViewIndex].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			m_ClearAttachments[lastImageViewIndex].clearValue.color = { { 0.0f, 0.0f, 0.0f, 1.0f} };
			m_ClearAttachments[lastImageViewIndex].colorAttachment = lastImageViewIndex;
			if(info.Name != "")
				m_AttachmentsMap[info.Name] = lastImageViewIndex;

			lastImageViewIndex++;
		}

		// Resolve if MSAA enabled
		if (m_Specification.bUseMSAA && !m_Specification.bTargetsSwapchain)
		{
			VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			AddAttachment(width, height, VK_SAMPLE_COUNT_1_BIT, usage, m_ColorFormat,
				m_ResolveAttachment.AttachmentVkInfo.image, m_ResolveAttachment.AttachmentVkInfo.view,
				m_ResolveAttachment.AttachmentVkInfo.mem);

			m_ResolveAttachment.ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			m_ResolveAttachment.ImageInfo.imageView = m_ResolveAttachment.AttachmentVkInfo.view;
			m_ResolveAttachment.ImageInfo.sampler = m_Sampler;

			auto cmd = VulkanCommandBuffer::CreateSingleCommandBuffer();
			VulkanTexture::SetImageLayout(cmd, m_ResolveAttachment.AttachmentVkInfo.image,
				VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			VulkanCommandBuffer::EndSingleCommandBuffer(cmd);

			if (m_Specification.bUsedByImGui)
				m_ResolveAttachment.ImGuiID = ImGui_ImplVulkan_AddTexture(m_ResolveAttachment.ImageInfo);

			attachments[lastImageViewIndex] = m_ResolveAttachment.AttachmentVkInfo.view;
			m_AttachmentsMap["Resolve"] = lastImageViewIndex;

			m_ClearValues[lastImageViewIndex].color = { { 0.0f, 0.0f, 0.0f, 1.0f} };
			m_ClearAttachments[lastImageViewIndex].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			m_ClearAttachments[lastImageViewIndex].clearValue.color = { { 0.1f, 0.1f, 0.1f, 1.0f} };
			lastImageViewIndex++;

		}

		if (m_Specification.bTargetsSwapchain)
		{
			m_ClearValues[lastImageViewIndex].color = { { 0.0f, 0.0f, 0.0f, 1.0f} };
			m_ClearAttachments[lastImageViewIndex].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			m_ClearAttachments[lastImageViewIndex].clearValue.color = { { 0.1f, 0.1f, 0.1f, 1.0f} };
			lastImageViewIndex++;
		}

		// Depth stencil attachment
		{
			VkImageUsageFlags usage = m_Specification.bUseMSAA ? VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
				: VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

			VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			AddAttachment(width, height, m_MSAASamples, usage, m_DepthFormat,
				m_DepthAttachment.AttachmentVkInfo.image, m_DepthAttachment.AttachmentVkInfo.view, m_DepthAttachment.AttachmentVkInfo.mem, imageAspect);

			attachments[lastImageViewIndex] = m_DepthAttachment.AttachmentVkInfo.view;

			m_ClearValues[lastImageViewIndex].depthStencil = { 1.0f, 0 };
			m_ClearAttachments[lastImageViewIndex].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			m_ClearAttachments[lastImageViewIndex].clearValue.depthStencil = { 1.0f, 0 };
		}

		// Render pass
		if (m_RenderPass == VK_NULL_HANDLE)
		{
			RenderPassGenInfo renderPassGenInfo = {};
			{
				renderPassGenInfo.ColorFormat = m_ColorFormat;
				renderPassGenInfo.DepthFormat = m_DepthFormat;
				renderPassGenInfo.MSAASamples = m_MSAASamples;
				renderPassGenInfo.NumColorAttachments = static_cast<uint32_t>(m_Attachments.size());
				renderPassGenInfo.NumDepthAttachments = 1;
				renderPassGenInfo.NumResolveAttachments = m_Specification.bUseMSAA ? 1 : 0;
			}

			VulkanRenderPass::Create(&m_Specification, &renderPassGenInfo, m_RenderPass);
		}

		VkFramebufferCreateInfo fbufCreateInfo = {};
		{
			fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			fbufCreateInfo.pNext = NULL;
			fbufCreateInfo.renderPass = m_RenderPass;
			fbufCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			fbufCreateInfo.pAttachments = attachments.data();
			fbufCreateInfo.width = m_Specification.Width;
			fbufCreateInfo.height = m_Specification.Height;
			fbufCreateInfo.layers = 1;
		}

		if (!m_Specification.bTargetsSwapchain)
		{
			m_VkFrameBuffers.resize(1);
			VK_CHECK_RESULT(vkCreateFramebuffer(m_Device, &fbufCreateInfo, nullptr, &m_VkFrameBuffers[0]));
		}
		else
		{
			uint32_t count = VulkanContext::GetSwapchain().m_ImageCount;
			m_VkFrameBuffers.resize(count);
			for (uint32_t i = 0; i < count; ++i)
			{
				uint32_t index = m_Specification.bUseMSAA ? 1 : 0;
				attachments[index] = VulkanContext::GetSwapchain().m_Buffers[i].View;
				VK_CHECK_RESULT(vkCreateFramebuffer(m_Device, &fbufCreateInfo, nullptr, &m_VkFrameBuffers[i]));
			}
		}

		return true;
	}

	// Temp Implmetation
	bool VulkanFramebuffer::CreateOmni(uint32_t width, uint32_t height)
	{
		m_Attachments.resize(2);
		m_Specification.Attachments = { FramebufferAttachment(AttachmentFormat::Color, true) };

		VkFormat colorFormat = VK_FORMAT_R32_SFLOAT;
		auto& fb = m_Attachments[0].AttachmentVkInfo;
		auto& cube = m_Attachments[1].AttachmentVkInfo;

		// Framebuffer - Attachment 1
		{
			VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			fb.image = VulkanTexture::CreateVkImage(m_Specification.Width, m_Specification.Height,
				1,
				VK_SAMPLE_COUNT_1_BIT,
				colorFormat,
				VK_IMAGE_TILING_OPTIMAL,
				usage, fb.mem);

			VkImageViewCreateInfo imageCI = {};
			imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageCI.format = colorFormat;
			imageCI.flags = 0;
			imageCI.subresourceRange = {};
			imageCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageCI.subresourceRange.baseMipLevel = 0;
			imageCI.subresourceRange.levelCount = 1;
			imageCI.subresourceRange.baseArrayLayer = 0;
			imageCI.subresourceRange.layerCount = 1;

			VkCommandBuffer cmdBuffer = VulkanCommandBuffer::CreateSingleCommandBuffer();
			VulkanTexture::SetImageLayout(cmdBuffer, fb.image,
				VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

			imageCI.image = fb.image;
			VK_CHECK_RESULT(vkCreateImageView(m_Device, &imageCI, nullptr, &fb.view));

			// Depth
			usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			m_DepthAttachment.AttachmentVkInfo.image = VulkanTexture::CreateVkImage(m_Specification.Width, m_Specification.Height,
				1,
				VK_SAMPLE_COUNT_1_BIT,
				m_DepthFormat,
				VK_IMAGE_TILING_OPTIMAL,
				usage, m_DepthAttachment.AttachmentVkInfo.mem, imageAspect);

			VulkanTexture::SetImageLayout(cmdBuffer, m_DepthAttachment.AttachmentVkInfo.image,
				VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

			VulkanCommandBuffer::EndSingleCommandBuffer(cmdBuffer);

			imageCI.format = m_DepthFormat;
			imageCI.subresourceRange = {};
			imageCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			imageCI.subresourceRange.baseMipLevel = 0;
			imageCI.subresourceRange.levelCount = 1;
			imageCI.subresourceRange.baseArrayLayer = 0;
			imageCI.subresourceRange.layerCount = 1;
			imageCI.image = m_DepthAttachment.AttachmentVkInfo.image;
			VK_CHECK_RESULT(vkCreateImageView(m_Device, &imageCI, nullptr, &m_DepthAttachment.AttachmentVkInfo.view));


			// Render pass
			if (m_RenderPass == VK_NULL_HANDLE)
			{
				VkAttachmentDescription osAttachments[2] = {};

				osAttachments[0].format = colorFormat;
				osAttachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
				osAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				osAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				osAttachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				osAttachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				osAttachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				osAttachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				// Depth attachment
				osAttachments[1].format = m_DepthFormat;
				osAttachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
				osAttachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				osAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				osAttachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				osAttachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				osAttachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				osAttachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				VkAttachmentReference colorReference = {};
				colorReference.attachment = 0;
				colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				VkAttachmentReference depthReference = {};
				depthReference.attachment = 1;
				depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				VkSubpassDescription subpass = {};
				subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
				subpass.colorAttachmentCount = 1;
				subpass.pColorAttachments = &colorReference;
				subpass.pDepthStencilAttachment = &depthReference;

				VkRenderPassCreateInfo renderPassCreateInfo = {};
				renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				renderPassCreateInfo.attachmentCount = 2;
				renderPassCreateInfo.pAttachments = osAttachments;
				renderPassCreateInfo.subpassCount = 1;
				renderPassCreateInfo.pSubpasses = &subpass;

				VK_CHECK_RESULT(vkCreateRenderPass(m_Device, &renderPassCreateInfo, nullptr, &m_RenderPass));
			}

			// Framebuffer
			{
				VkImageView attachments[2];
				attachments[0] = fb.view;
				attachments[1] = m_DepthAttachment.AttachmentVkInfo.view;

				VkFramebufferCreateInfo fbufCreateInfo = {};
				{
					fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
					fbufCreateInfo.pNext = NULL;
					fbufCreateInfo.renderPass = m_RenderPass;
					fbufCreateInfo.attachmentCount = 2;
					fbufCreateInfo.pAttachments = attachments;
					fbufCreateInfo.width = m_Specification.Width;
					fbufCreateInfo.height = m_Specification.Height;
					fbufCreateInfo.layers = 1;
				}

				m_VkFrameBuffers.resize(1);
				VK_CHECK_RESULT(vkCreateFramebuffer(m_Device, &fbufCreateInfo, nullptr, &m_VkFrameBuffers[0]));
			}
		}

		// Omni Cube - Attachment 2
		{
			// 32 bit float format for higher precision
			VkFormat format = VK_FORMAT_R32_SFLOAT;
			VkCommandBuffer cmdBuffer = VulkanCommandBuffer::CreateSingleCommandBuffer();
			{
				VkImageCreateInfo imageCI = {};
				{
					imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
					imageCI.imageType = VK_IMAGE_TYPE_2D;
					imageCI.format = format;
					imageCI.mipLevels = 1;
					imageCI.extent.depth = 1;
					imageCI.arrayLayers = 6;
					imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
					imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
					imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
					imageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					imageCI.extent = { (uint32_t)width, (uint32_t)height, 1 };
					imageCI.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
					imageCI.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

					VK_CHECK_RESULT(vkCreateImage(m_Device, &imageCI, nullptr, &cube.image));
				}

				VkMemoryRequirements memReqs;
				vkGetImageMemoryRequirements(m_Device, cube.image, &memReqs);
				uint32_t typeIndex = VulkanContext::GetDevice().GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
				VulkanMemoryAllocator::Allocate(m_Device, memReqs, &cube.mem, typeIndex);
				VK_CHECK_RESULT(vkBindImageMemory(m_Device, cube.image, cube.mem, 0));
			}

			// Image barrier for optimal image (target)
			VkImageSubresourceRange subresourceRange = {};
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = 1;
			subresourceRange.layerCount = 6;
			VulkanTexture::SetImageLayout(cmdBuffer, cube.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);
			VulkanCommandBuffer::EndSingleCommandBuffer(cmdBuffer);

			// Sampler
			{
				VkSamplerCreateInfo samplerCI = {};
				{
					samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
					samplerCI.magFilter = VK_FILTER_LINEAR;
					samplerCI.minFilter = VK_FILTER_LINEAR;
					samplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
					samplerCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
					samplerCI.addressModeV = samplerCI.addressModeU;
					samplerCI.addressModeW = samplerCI.addressModeU;
					samplerCI.mipLodBias = 0.0f;
					samplerCI.maxAnisotropy = 1.0f;
					samplerCI.compareOp = VK_COMPARE_OP_NEVER;
					samplerCI.minLod = 0.0f;
					samplerCI.maxLod = 1.0f;
					samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

					VK_CHECK_RESULT(vkCreateSampler(m_Device, &samplerCI, nullptr, &m_Sampler));
				}
			}

			// Image View
			{
				VkImageViewCreateInfo view = {};
				{
					view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
					view.image = VK_NULL_HANDLE;
					view.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
					view.format = format;
					view.components = { VK_COMPONENT_SWIZZLE_R };
					view.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
					view.subresourceRange.layerCount = 6;
					view.image = cube.image;

					VK_CHECK_RESULT(vkCreateImageView(m_Device, &view, nullptr, &cube.view));
				}
			}

			m_Attachments[1].ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			m_Attachments[1].ImageInfo.imageView = cube.view;
			m_Attachments[1].ImageInfo.sampler = m_Sampler;
		}
		
		m_ClearValues.resize(2);
		m_ClearAttachments.resize(2);

		m_ClearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f} };
		m_ClearAttachments[0].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		m_ClearAttachments[0].clearValue.color = { { 0.0f, 0.0f, 0.0f, 1.0f} };
		m_ClearAttachments[0].colorAttachment = 0;

		m_ClearValues[1].depthStencil = { 1.0f, 0 };
		m_ClearAttachments[1].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		m_ClearAttachments[1].clearValue.depthStencil = { 1.0f, 0 };

		return true;
	}

	bool VulkanFramebuffer::CreateShadow(uint32_t width, uint32_t height)
	{
		// Depth 
		{
			VkImageUsageFlags usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT;

			AddAttachment(width, height, VK_SAMPLE_COUNT_1_BIT, usage, m_DepthFormat,
				m_DepthAttachment.AttachmentVkInfo.image, m_DepthAttachment.AttachmentVkInfo.view, m_DepthAttachment.AttachmentVkInfo.mem, imageAspect);
		}

		auto cmd = VulkanCommandBuffer::CreateSingleCommandBuffer();
		VulkanTexture::SetImageLayout(cmd, m_DepthAttachment.AttachmentVkInfo.image,
			VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
		VulkanCommandBuffer::EndSingleCommandBuffer(cmd);

		// Sampler
		{
			VkFilter shadowmap_filter = IsFormatIsFilterable(VulkanContext::GetDevice().GetPhysicalDevice(), m_DepthFormat, VK_IMAGE_TILING_OPTIMAL) ?
				VK_FILTER_LINEAR : VK_FILTER_NEAREST;

			VkSamplerCreateInfo sampler = {};
			sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			sampler.maxAnisotropy = 1.0f;
			sampler.magFilter = shadowmap_filter;
			sampler.minFilter = shadowmap_filter;
			sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			sampler.addressModeV = sampler.addressModeU;
			sampler.addressModeW = sampler.addressModeU;
			sampler.mipLodBias = 0.0f;
			sampler.maxAnisotropy = 1.0f;
			sampler.minLod = 0.0f;
			sampler.maxLod = 1.0f;
			sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			VK_CHECK_RESULT(vkCreateSampler(m_Device, &sampler, nullptr, &m_Sampler));
		}

		// Render Pass
		{
			VkAttachmentDescription attachmentDescription{};
			attachmentDescription.format = m_DepthFormat;
			attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
			attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;							// Clear depth at beginning of the render pass
			attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;						// We will read from depth, so it's important to store the depth attachment results
			attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;					// We don't care about initial layout of the attachment
			attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;// Attachment will be transitioned to shader read at render pass end

			VkAttachmentReference depthReference = {};
			depthReference.attachment = 0;
			depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;			// Attachment will be used as depth/stencil during render pass

			VkSubpassDescription subpass = {};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 0;													// No color attachments
			subpass.pDepthStencilAttachment = &depthReference;									// Reference to our depth attachment

			// Use subpass dependencies for layout transitions
			std::array<VkSubpassDependency, 2> dependencies;

			dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[0].dstSubpass = 0;
			dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			dependencies[1].srcSubpass = 0;
			dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			VkRenderPassCreateInfo renderPassCreateInfo = {};
			renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassCreateInfo.attachmentCount = 1;
			renderPassCreateInfo.pAttachments = &attachmentDescription;
			renderPassCreateInfo.subpassCount = 1;
			renderPassCreateInfo.pSubpasses = &subpass;
			renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
			renderPassCreateInfo.pDependencies = dependencies.data();

			VK_CHECK_RESULT(vkCreateRenderPass(m_Device, &renderPassCreateInfo, nullptr, &m_RenderPass));
		}

		// Framebuffer
		{
			VkFramebufferCreateInfo fbufCreateInfo = {};
			fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			fbufCreateInfo.renderPass = m_RenderPass;
			fbufCreateInfo.attachmentCount = 1;
			fbufCreateInfo.pAttachments = &m_DepthAttachment.AttachmentVkInfo.view;
			fbufCreateInfo.width = width;
			fbufCreateInfo.height = height;
			fbufCreateInfo.layers = 1;

			m_VkFrameBuffers.resize(1);
			VK_CHECK_RESULT(vkCreateFramebuffer(m_Device, &fbufCreateInfo, nullptr, &m_VkFrameBuffers[0]));
		}


		m_DepthAttachment.ImageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		m_DepthAttachment.ImageInfo.sampler = m_Sampler;
		m_DepthAttachment.ImageInfo.imageView = m_DepthAttachment.AttachmentVkInfo.view;

		m_ClearValues.resize(1);
		m_ClearAttachments.resize(1);

		m_ClearValues[0].depthStencil = { 1.0f, 0 };
		m_ClearAttachments[0].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		m_ClearAttachments[0].clearValue.depthStencil = { 1.0f, 0 };

		return true;
	}

	void VulkanFramebuffer::CreateSampler(VkFilter filter)
	{
		VkSamplerCreateInfo samplerCI = {};
		{
			auto& device = VulkanContext::GetDevice();

			samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerCI.magFilter = filter;
			samplerCI.minFilter = filter;
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
		m_Specification = data;
		m_MSAASamples = data.bUseMSAA ? VulkanContext::GetDevice().GetMSAASamplesCount(): VK_SAMPLE_COUNT_1_BIT;

		switch (m_Specification.Specialisation)
		{
		case FramebufferSpecialisation::None:
		{
			return Create(data.Width, data.Height);
		}
		case FramebufferSpecialisation::ShadowMap:
		{
			return CreateShadow(data.Width, data.Height);
		}
		case FramebufferSpecialisation::OmniShadow:
		{
			return CreateOmni(data.Width, data.Height);
		}
		default:
			return false;
		}
	}

	void VulkanFramebuffer::OnResize(uint32_t width, uint32_t height)
	{
		if (m_Specification.bResizable)
		{
			FreeResources();
			Create(width, height);
		}
	}

	void VulkanFramebuffer::FreeResources()
	{
		for (auto& color : m_Attachments)
		{
			FreeAttachment(color);
		}

		FreeAttachment(m_ResolveAttachment);
		FreeAttachment(m_DepthAttachment);

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

	void VulkanFramebuffer::FreeAttachment(Attachment& framebuffer)
	{
		if (framebuffer.AttachmentVkInfo.view != VK_NULL_HANDLE)
			vkDestroyImageView(m_Device, framebuffer.AttachmentVkInfo.view, nullptr);

		if (framebuffer.AttachmentVkInfo.image != VK_NULL_HANDLE)
			vkDestroyImage(m_Device, framebuffer.AttachmentVkInfo.image, nullptr);

		if (framebuffer.AttachmentVkInfo.mem != VK_NULL_HANDLE)
			vkFreeMemory(m_Device, framebuffer.AttachmentVkInfo.mem, nullptr);
	}

	VkBool32 VulkanFramebuffer::IsFormatIsFilterable(VkPhysicalDevice physicalDevice, VkFormat format, VkImageTiling tiling)
	{
		VkFormatProperties formatProps;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);

		if (tiling == VK_IMAGE_TILING_OPTIMAL)
			return formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

		if (tiling == VK_IMAGE_TILING_LINEAR)
			return formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

		return false;
	}

	VkFormat VulkanFramebuffer::GetAttachmentFormat(AttachmentFormat format)
	{
		switch (format)
		{

		case SmolEngine::AttachmentFormat::UNORM_8:			    return VK_FORMAT_R8_UNORM;
		case SmolEngine::AttachmentFormat::UNORM2_8: 			return VK_FORMAT_R8G8_UNORM;
		case SmolEngine::AttachmentFormat::UNORM3_8: 			return VK_FORMAT_R8G8B8_UNORM;
		case SmolEngine::AttachmentFormat::UNORM4_8: 			return VK_FORMAT_R8G8B8A8_UNORM;

		case SmolEngine::AttachmentFormat::UNORM_16:			return VK_FORMAT_R16_UNORM;
		case SmolEngine::AttachmentFormat::UNORM2_16: 			return VK_FORMAT_R16G16_UNORM;
		case SmolEngine::AttachmentFormat::UNORM3_16: 			return VK_FORMAT_R16G16B16_UNORM;
		case SmolEngine::AttachmentFormat::UNORM4_16: 			return VK_FORMAT_R16G16B16A16_UNORM;

		case SmolEngine::AttachmentFormat::SFloat_16: 			return VK_FORMAT_R16_SFLOAT;
		case SmolEngine::AttachmentFormat::SFloat2_16: 			return VK_FORMAT_R16G16_SFLOAT;
		case SmolEngine::AttachmentFormat::SFloat3_16: 			return VK_FORMAT_R16G16B16_SFLOAT;
		case SmolEngine::AttachmentFormat::SFloat4_16: 			return VK_FORMAT_R16G16B16A16_SFLOAT;

		case SmolEngine::AttachmentFormat::SFloat_32: 			return VK_FORMAT_R32_SFLOAT;
		case SmolEngine::AttachmentFormat::SFloat2_32: 			return VK_FORMAT_R32G32_SFLOAT;
		case SmolEngine::AttachmentFormat::SFloat3_32: 			return VK_FORMAT_R32G32B32_SFLOAT;
		case SmolEngine::AttachmentFormat::SFloat4_32: 			return VK_FORMAT_R32G32B32A32_SFLOAT;

		case SmolEngine::AttachmentFormat::SInt_8: 			    return VK_FORMAT_R8_SINT;
		case SmolEngine::AttachmentFormat::SInt2_8:			    return VK_FORMAT_R8G8_SINT;
		case SmolEngine::AttachmentFormat::SInt3_8:			    return VK_FORMAT_R8G8B8_SINT;
		case SmolEngine::AttachmentFormat::SInt4_8:			    return VK_FORMAT_R8G8B8A8_SINT;
															    
		case SmolEngine::AttachmentFormat::SInt_16: 			return VK_FORMAT_R16_SINT;
		case SmolEngine::AttachmentFormat::SInt2_16:			return VK_FORMAT_R16G16_SINT;
		case SmolEngine::AttachmentFormat::SInt3_16:			return VK_FORMAT_R16G16B16_SINT;
		case SmolEngine::AttachmentFormat::SInt4_16:			return VK_FORMAT_R16G16B16A16_SINT;
															    
		case SmolEngine::AttachmentFormat::SInt_32: 			return VK_FORMAT_R32_SINT;
		case SmolEngine::AttachmentFormat::SInt2_32:			return VK_FORMAT_R32G32_SINT;
		case SmolEngine::AttachmentFormat::SInt3_32:			return VK_FORMAT_R32G32B32_SINT;
		case SmolEngine::AttachmentFormat::SInt4_32:			return VK_FORMAT_R32G32B32A32_SINT;

		case SmolEngine::AttachmentFormat::Color:			    return VulkanContext::GetSwapchain().GetColorFormat();
		case SmolEngine::AttachmentFormat::Depth:               return VulkanContext::GetSwapchain().GetDepthFormat();
		default:
			break;
		}

		return VulkanContext::GetSwapchain().GetColorFormat();
	}

	VkRenderPass VulkanFramebuffer::GetRenderPass() const
	{
		return m_RenderPass;
	}

	VkSampleCountFlagBits VulkanFramebuffer::GetMSAASamples() const
	{
		return m_MSAASamples;
	}

	const uint32_t VulkanFramebuffer::GetAttachmentCount() const
	{
		return static_cast<uint32_t>(m_Attachments.size());
	}

	Attachment* VulkanFramebuffer::GetAttachment(uint32_t index)
	{
		if (m_Specification.bUseMSAA)
			return &m_ResolveAttachment;

		return &m_Attachments[index];
	}

	Attachment* VulkanFramebuffer::GetAttachment(std::string& name)
	{
		const auto& it = m_AttachmentsMap.find(name);
		if (it != m_AttachmentsMap.end())
			return &m_Attachments[it->second];

		return nullptr;
	}

	Attachment* VulkanFramebuffer::GetDethAttachment()
	{
		return &m_DepthAttachment;
	}

	void VulkanFramebuffer::SetClearColors(const glm::vec4& clearColors)
	{
		for (auto& clearAttachment : m_ClearAttachments)
		{
			if (clearAttachment.aspectMask && VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)
				clearAttachment.clearValue.depthStencil = { 1.0f, 0 };
			else
				clearAttachment.clearValue = { { clearColors.r,  clearColors.g,  clearColors.b,  clearColors.a } };
		}
	}

	const std::vector<VkClearAttachment>& VulkanFramebuffer::GetClearAttachments() const
	{
		return m_ClearAttachments;
	}

	const std::vector<VkClearValue>& VulkanFramebuffer::GetClearValues() const
	{
		return m_ClearValues;
	}

	const FramebufferSpecification& VulkanFramebuffer::GetSpecification() const
	{
		return m_Specification;
	}

	const VkFramebuffer VulkanFramebuffer::GetCurrentVkFramebuffer() const
	{
		uint32_t index;
		m_Specification.bTargetsSwapchain ? index = VulkanContext::GetSwapchain().GetCurrentBufferIndex() :
			index = 0;
		return m_VkFrameBuffers[index];
	}

	const VkFramebuffer VulkanFramebuffer::GetVkFramebuffer(uint32_t index) const
	{
		return m_VkFrameBuffers[index];
	}
}