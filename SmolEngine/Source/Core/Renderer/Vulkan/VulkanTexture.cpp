#include "stdafx.h"
#include "VulkanTexture.h"
#include "stb_image.h"

#include "Core/Renderer/Vulkan/VulkanContext.h"
#include "Core/Renderer/Vulkan/VulkanStagingBuffer.h"
#include "Core/Renderer/Vulkan/VulkanMemoryAllocator.h"

#include "../Libraries/imgui/examples/imgui_impl_vulkan.h"

namespace SmolEngine
{
	VulkanTexture::VulkanTexture()
	{
		m_Device = *VulkanContext::GetDevice().GetLogicalDevice();
	}

	VulkanTexture::~VulkanTexture()
	{
		if (m_Device)
		{
			vkDestroyImage(m_Device, m_Image, nullptr);
			vkDestroyImageView(m_Device, m_ImageView, nullptr);
			vkDestroySampler(m_Device, m_Samper, nullptr);
			vkFreeMemory(m_Device, m_DeviceMemory, nullptr);
		}
	}

	void VulkanTexture::CreateTexture2D(uint32_t width, uint32_t height)
	{

	}

	void VulkanTexture::CreateTexture2D(const std::string& filePath)
	{
		int height, width, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
			if (!data)
			{
				NATIVE_ERROR("VulkanTexture:: Texture not found! file: {}, line: {}", __FILE__, __LINE__);
				abort();
			}
		}

		assert(channels == 4);

		VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
		VkDeviceSize size = width * height * 4;

		VulkanStagingBuffer stagingBuffer;
		stagingBuffer.Create(size);
		stagingBuffer.SetData(data, size);

		VkBufferImageCopy bufferImageCopy = {};
		{
			bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			bufferImageCopy.imageSubresource.mipLevel = 0;
			bufferImageCopy.imageSubresource.baseArrayLayer = 0;
			bufferImageCopy.imageSubresource.layerCount = 1;
			bufferImageCopy.imageExtent.width = width;
			bufferImageCopy.imageExtent.height = height;
			bufferImageCopy.imageExtent.depth = 1;
			bufferImageCopy.bufferOffset = 0;
		}

		VkImageCreateInfo imageCI = {};
		{
			imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCI.imageType = VK_IMAGE_TYPE_2D;
			imageCI.format = format;
			imageCI.mipLevels = 1;
			imageCI.arrayLayers = 1;
			imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
			imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageCI.extent = { (uint32_t)width, (uint32_t)height, 1 };
			imageCI.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

			VK_CHECK_RESULT(vkCreateImage(m_Device, &imageCI, nullptr, &m_Image));
		}


		VkMemoryRequirements memReqs;
		vkGetImageMemoryRequirements(m_Device, m_Image, &memReqs);
		uint32_t typeIndex = VulkanContext::GetDevice().GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VulkanMemoryAllocator::Allocate(m_Device, memReqs, &m_DeviceMemory, typeIndex);
		VK_CHECK_RESULT(vkBindImageMemory(m_Device, m_Image, m_DeviceMemory, 0));


		VkImageSubresourceRange subResRange = {};
		{
			subResRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subResRange.baseMipLevel = 0;
			subResRange.levelCount = 1;
			subResRange.layerCount = 1;
		}

		VkImageMemoryBarrier imageMemBarries = {};
		{
			imageMemBarries.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemBarries.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemBarries.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemBarries.image = m_Image;
			imageMemBarries.subresourceRange = subResRange;
			imageMemBarries.srcAccessMask = 0;
			imageMemBarries.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageMemBarries.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageMemBarries.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		}

		VulkanCommandBuffer& vulkanCmdBuffer = VulkanContext::GetCommandBuffer();
		VkCommandBuffer copyCmd = vulkanCmdBuffer.CreateSingleCommandBuffer(false);
		VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_HOST_BIT;
		VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		{
			vkCmdPipelineBarrier(copyCmd, srcStage, dstStage,
				0,
				0, nullptr,
				0, nullptr, 
				1, &imageMemBarries);

			vkCmdCopyBufferToImage(copyCmd, stagingBuffer.GetBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopy);

			// Once the data has been uploaded we transfer to the texture image to the shader read layout, so it can be sampled from
			imageMemBarries.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageMemBarries.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			imageMemBarries.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageMemBarries.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemBarries);
		}

		m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		vulkanCmdBuffer.FlushCommandBuffer(copyCmd);
		stagingBuffer.Destroy();


		/// Samplers
		/// https://vulkan-tutorial.com/Texture_mapping/Image_view_and_sampler

		VkSamplerCreateInfo samplerCI = {};
		{
			samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerCI.maxAnisotropy = 1.0f;
			samplerCI.magFilter = VK_FILTER_LINEAR;
			samplerCI.minFilter = VK_FILTER_LINEAR;
			samplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerCI.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerCI.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerCI.mipLodBias = 0.0f;
			samplerCI.compareOp = VK_COMPARE_OP_NEVER;
			samplerCI.minLod = 0.0f;
			samplerCI.maxLod = 1.0f;
			samplerCI.anisotropyEnable = VK_FALSE;
			samplerCI.maxAnisotropy = 1.0;
			samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

			VK_CHECK_RESULT(vkCreateSampler(m_Device, &samplerCI, nullptr, &m_Samper));
		}

		/// Image View
		/// https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Image_views

		VkImageViewCreateInfo imageViewCI = {};
		{
			imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCI.format = format;
			imageViewCI.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
			imageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewCI.subresourceRange.baseMipLevel = 0;
			imageViewCI.subresourceRange.baseArrayLayer = 0;
			imageViewCI.subresourceRange.layerCount = 1;
			imageViewCI.subresourceRange.levelCount = 1;
			imageViewCI.image = m_Image;

			VK_CHECK_RESULT(vkCreateImageView(m_Device, &imageViewCI, nullptr, &m_ImageView));
		}

		m_Width = width;
		m_Height = height;
		m_FilePath = filePath;
		m_IsCreated = true;

		m_DescriptorImageInfo = {};
		m_DescriptorImageInfo.imageLayout = m_ImageLayout;
		m_DescriptorImageInfo.imageView = m_ImageView;
		m_DescriptorImageInfo.sampler = m_Samper;

#ifdef SMOLENGINE_EDITOR

		m_ImGuiTextureID = ImGui_ImplVulkan_AddTexture(m_DescriptorImageInfo);

#endif // SMOLENGINE_EDITOR

		stbi_image_free(data);
	}

	void VulkanTexture::CreateCubeTexture()
	{

	}

	uint32_t VulkanTexture::GetHeight() const
	{
		return m_Height;
	}

	uint32_t VulkanTexture::GetWidth() const
	{
		return m_Width;
	}

	bool VulkanTexture::IsActive() const
	{
		return m_IsCreated;
	}
}