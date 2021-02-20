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
			}

			if (m_Specification.bUsedByImGui)
			{
				vkInfo.ImGuiID = ImGui_ImplVulkan_AddTexture(vkInfo.ImageInfo);
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

			if (m_Specification.bUsedByImGui)
			{
				m_ResolveAttachment.ImGuiID = ImGui_ImplVulkan_AddTexture(m_ResolveAttachment.ImageInfo);
			}

			attachments[lastImageViewIndex] = m_ResolveAttachment.AttachmentVkInfo.view;
			m_AttachmentsMap["Resolve"] = lastImageViewIndex;
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
	bool VulkanFramebuffer::CreateCascade(uint32_t width, uint32_t height)
	{
		// 	Depth map renderpass
		{
			VkAttachmentDescription attachmentDescription{};
			attachmentDescription.format = m_DepthFormat;
			attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
			attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

			m_ClearAttachments.resize(1);
			m_ClearValues.resize(1);
			m_ClearValues[0].depthStencil = { 1.0f, 0 };
			m_ClearAttachments[0].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			m_ClearAttachments[0].clearValue.depthStencil = { 1.0f, 0 };

			VkAttachmentReference depthReference = {};
			depthReference.attachment = 0;
			depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			VkSubpassDescription subpass = {};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 0;
			subpass.pDepthStencilAttachment = &depthReference;

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

		//Create Sampler
		CreateSampler(VK_FILTER_LINEAR);

		VkImageUsageFlags usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
		//Layered depth image and views
		{
			m_DepthAttachment.AttachmentVkInfo.image = VulkanTexture::CreateVkImage(width, height,
				1,
				VK_SAMPLE_COUNT_1_BIT,
				m_DepthFormat,
				VK_IMAGE_TILING_OPTIMAL,
				usage, m_DepthAttachment.AttachmentVkInfo.mem, m_Specification.NumArrayLayers);

			VkImageViewCreateInfo imageViewCI = {};
			{
				imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
				imageViewCI.format = m_DepthFormat;
				imageViewCI.subresourceRange = {};
				imageViewCI.subresourceRange.aspectMask = imageAspect;
				imageViewCI.subresourceRange.baseMipLevel = 0;
				imageViewCI.subresourceRange.baseArrayLayer = 0;
				imageViewCI.subresourceRange.levelCount = 1;
				imageViewCI.subresourceRange.layerCount = m_Specification.NumArrayLayers;
				imageViewCI.image = m_DepthAttachment.AttachmentVkInfo.image;

				VK_CHECK_RESULT(vkCreateImageView(m_Device, &imageViewCI, nullptr, &m_DepthAttachment.AttachmentVkInfo.view));
			}

			m_DepthAttachment.ImageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			m_DepthAttachment.ImageInfo.sampler = m_Sampler;
			m_DepthAttachment.ImageInfo.imageView = m_DepthAttachment.AttachmentVkInfo.view;
		}

		// One image and framebuffer per cascade
		{
			m_Attachments.resize(m_Specification.NumArrayLayers);
			m_VkFrameBuffers.resize(m_Specification.NumArrayLayers);
			for (uint32_t i = 0; i < m_Specification.NumArrayLayers; ++i)
			{
				VkImageViewCreateInfo imageViewCI = {};
				{
					imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
					imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
					imageViewCI.format = m_DepthFormat;
					imageViewCI.subresourceRange = {};
					imageViewCI.subresourceRange.aspectMask = imageAspect;
					imageViewCI.subresourceRange.baseMipLevel = 0;
					imageViewCI.subresourceRange.baseArrayLayer = i;
					imageViewCI.subresourceRange.levelCount = 1;
					imageViewCI.subresourceRange.layerCount = 1;
					imageViewCI.image = m_DepthAttachment.AttachmentVkInfo.image;

					VK_CHECK_RESULT(vkCreateImageView(m_Device, &imageViewCI, nullptr, &m_Attachments[i].AttachmentVkInfo.view));
				}

				m_Attachments[i].ImageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
				m_Attachments[i].ImageInfo.imageView = m_Attachments[i].AttachmentVkInfo.view;
				m_Attachments[i].ImageInfo.sampler = m_Sampler;

				VkFramebufferCreateInfo fbufCreateInfo = {};
				{
					fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
					fbufCreateInfo.pNext = NULL;
					fbufCreateInfo.renderPass = m_RenderPass;
					fbufCreateInfo.attachmentCount = 1;
					fbufCreateInfo.pAttachments = &m_Attachments[i].AttachmentVkInfo.view;
					fbufCreateInfo.width = m_Specification.Width;
					fbufCreateInfo.height = m_Specification.Height;
					fbufCreateInfo.layers = 1;
				}

				VK_CHECK_RESULT(vkCreateFramebuffer(m_Device, &fbufCreateInfo, nullptr, &m_VkFrameBuffers[i]));
			}
		}

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

		return m_Specification.bUsingCascadeObject ? CreateCascade(data.Width, data.Height): Create(data.Width, data.Height);
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
		auto& it = m_AttachmentsMap.find(name);
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
			if (clearAttachment.aspectMask == VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)
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