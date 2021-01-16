#include "stdafx.h"
#include "VulkanTexture.h"
#include "stb_image.h"
#include "stb_image_write.h"

#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanStagingBuffer.h"
#include "Renderer/Vulkan/VulkanMemoryAllocator.h"

#include "../Libraries/imgui/examples/imgui_impl_vulkan.h"

namespace SmolEngine
{
	static 	VkFormat Format = VK_FORMAT_R8G8B8A8_UNORM;

	VulkanTexture::VulkanTexture()
	{
		m_Device = VulkanContext::GetDevice().GetLogicalDevice();
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

	void VulkanTexture::CreateWhiteTetxure(uint32_t width, uint32_t height)
	{
		uint32_t whiteTextureData = 0xffffffff;
		CreateTexture(width, height, { &whiteTextureData }, TextureType::Texture2D);

		m_Width = width;
		m_Height = height;
		m_IsCreated = true;
	}

	void VulkanTexture::CreateTexture(const std::string& filePath)
	{
		int height, width, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			data = stbi_load(filePath.c_str(), &width, &height, &channels, 4);
			if (!data)
			{
				NATIVE_ERROR("VulkanTexture:: Texture not found! file: {}, line: {}", __FILE__, __LINE__);
				abort();
			}
		}

		CreateTexture(width, height, { data }, TextureType::Texture2D);
		m_Width = width;
		m_Height = height;
		m_FilePath = filePath;
		m_IsCreated = true;

		stbi_image_free(data);
		std::hash<std::string> hasher;
		m_ID = hasher(filePath);
	}

	void VulkanTexture::CreateCubeMap(const std::array<std::string, 6>& filePaths)
	{
		int height, width, channels;
		stbi_uc* data[6];
		for (uint32_t i = 0; i < 6; ++i)
		{
			stbi_set_flip_vertically_on_load(1);
			{
				data[i] = stbi_load(filePaths[i].c_str(), &width, &height, &channels, 4);
				if (!data[i])
				{
					NATIVE_ERROR("VulkanTexture:: Texture not found! file: {}, line: {}", __FILE__, __LINE__);
					abort();
				}
			}
		}

		const uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
		const VkDeviceSize size = width * height * 4 * 6;
		const VkDeviceSize bufferOffset = size / 6;

		VkImageCreateInfo imageCI = {};
		{
			imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCI.imageType = VK_IMAGE_TYPE_2D;
			imageCI.format = Format;
			imageCI.mipLevels = 1;
			imageCI.arrayLayers = 6;
			imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
			imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageCI.extent = { (uint32_t)width, (uint32_t)height, 1 };
			imageCI.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			imageCI.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

			VK_CHECK_RESULT(vkCreateImage(m_Device, &imageCI, nullptr, &m_Image));
		}

		VkMemoryRequirements memReqs;
		vkGetImageMemoryRequirements(m_Device, m_Image, &memReqs);
		uint32_t typeIndex = VulkanContext::GetDevice().GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VulkanMemoryAllocator::Allocate(m_Device, memReqs, &m_DeviceMemory, typeIndex);
		VK_CHECK_RESULT(vkBindImageMemory(m_Device, m_Image, m_DeviceMemory, 0));

		VulkanStagingBuffer stagingBuffer;
		stagingBuffer.Create(size);

		std::vector< VkBufferImageCopy> bufferImages(6);
		VkDeviceSize offset = 0;
		uint32_t index = 0;
		for (uint32_t i = 0; i < 6; ++i)
		{
			stagingBuffer.SetData(data[i], width * height * 4, offset);

			VkBufferImageCopy bufferImageCopy = {};
			{
				bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				bufferImageCopy.imageSubresource.mipLevel = 1;
				bufferImageCopy.imageSubresource.baseArrayLayer = i;
				bufferImageCopy.imageSubresource.layerCount = 1;
				bufferImageCopy.imageExtent.width = width;
				bufferImageCopy.imageExtent.height = height;
				bufferImageCopy.imageExtent.depth = 1;
				bufferImageCopy.bufferOffset = offset;
			}

			bufferImages[i] = bufferImageCopy;
			offset += bufferOffset;
		}

		VkImageSubresourceRange subResRange = {};
		{
			subResRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subResRange.baseMipLevel = 0;
			subResRange.levelCount = 1;
			subResRange.layerCount = GetImageArrayLayers(TextureType::CubeMap);
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
			//vkCmdPipelineBarrier(copyCmd, srcStage, dstStage,
			//	0,
			//	0, nullptr,
			//	0, nullptr,
			//	1, &imageMemBarries);

			vkCmdCopyBufferToImage(copyCmd, stagingBuffer.GetBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				static_cast<uint32_t>(bufferImages.size()), bufferImages.data());

			// Once the data has been uploaded we transfer to the texture image to the shader read layout, so it can be sampled from
			imageMemBarries.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageMemBarries.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			imageMemBarries.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageMemBarries.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemBarries);
		}

		vulkanCmdBuffer.FlushCommandBuffer(copyCmd);
		stagingBuffer.Destroy();

		m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		CreateSamplerAndImageView(TextureType::CubeMap);
	}

	uint32_t VulkanTexture::GetHeight() const
	{
		return m_Height;
	}

	uint32_t VulkanTexture::GetWidth() const
	{
		return m_Width;
	}

	size_t VulkanTexture::GetID() const
	{
		return m_ID;
	}

	void* VulkanTexture::GetImGuiTextureID() const
	{
		return m_ImGuiTextureID;
	}

	bool VulkanTexture::IsActive() const
	{
		return m_IsCreated;
	}

	void VulkanTexture::CreateTexture(uint32_t width, uint32_t height, void* data, TextureType type)
	{
		const VkDeviceSize size = width * height * 4;

		VkImageCreateInfo imageCI = {};
		{
			imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCI.imageType = VK_IMAGE_TYPE_2D;
			imageCI.format = Format;
			imageCI.mipLevels = 1;
			imageCI.arrayLayers = GetImageArrayLayers(type);
			imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
			imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageCI.extent = { (uint32_t)width, (uint32_t)height, 1 };
			imageCI.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			if (type == TextureType::CubeMap)
			{
				imageCI.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
			}

			VK_CHECK_RESULT(vkCreateImage(m_Device, &imageCI, nullptr, &m_Image));
		}


		VkMemoryRequirements memReqs;
		vkGetImageMemoryRequirements(m_Device, m_Image, &memReqs);
		uint32_t typeIndex = VulkanContext::GetDevice().GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VulkanMemoryAllocator::Allocate(m_Device, memReqs, &m_DeviceMemory, typeIndex);
		VK_CHECK_RESULT(vkBindImageMemory(m_Device, m_Image, m_DeviceMemory, 0));

		VulkanStagingBuffer stagingBuffer;
		stagingBuffer.Create(size);
		stagingBuffer.SetData(data, size);

		VkBufferImageCopy bufferImageCopy = {};
		{
			bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			bufferImageCopy.imageSubresource.mipLevel = 0;
			bufferImageCopy.imageSubresource.baseArrayLayer = 0;
			bufferImageCopy.imageSubresource.layerCount = GetImageArrayLayers(type);
			bufferImageCopy.imageExtent.width = width;
			bufferImageCopy.imageExtent.height = height;
			bufferImageCopy.imageExtent.depth = 1;
			bufferImageCopy.bufferOffset = 0;
		}

		VkImageSubresourceRange subResRange = {};
		{
			subResRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subResRange.baseMipLevel = 0;
			subResRange.levelCount = 1;
			subResRange.layerCount = GetImageArrayLayers(type);
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

			vkCmdCopyBufferToImage(copyCmd, stagingBuffer.GetBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
				1, &bufferImageCopy);

			// Once the data has been uploaded we transfer to the texture image to the shader read layout, so it can be sampled from
			imageMemBarries.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageMemBarries.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			imageMemBarries.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageMemBarries.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemBarries);
		}

		vulkanCmdBuffer.FlushCommandBuffer(copyCmd);
		stagingBuffer.Destroy();

		m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		CreateSamplerAndImageView(type);

#ifdef SMOLENGINE_EDITOR

		m_ImGuiTextureID = ImGui_ImplVulkan_AddTexture(m_DescriptorImageInfo);

#endif // SMOLENGINE_EDITOR
	}

	void VulkanTexture::CreateSamplerAndImageView(TextureType type)
	{
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
			imageViewCI.viewType = GetVkImageViewType(type);
			imageViewCI.format = Format;
			imageViewCI.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
			imageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewCI.subresourceRange.baseMipLevel = 0;
			imageViewCI.subresourceRange.baseArrayLayer = 0;
			imageViewCI.subresourceRange.layerCount = GetImageArrayLayers(type);
			imageViewCI.subresourceRange.levelCount = 1;
			imageViewCI.image = m_Image;

			VK_CHECK_RESULT(vkCreateImageView(m_Device, &imageViewCI, nullptr, &m_ImageView));
		}

		m_DescriptorImageInfo = {};
		m_DescriptorImageInfo.imageLayout = m_ImageLayout;
		m_DescriptorImageInfo.imageView = m_ImageView;
		m_DescriptorImageInfo.sampler = m_Samper;
	}

	VkImageViewType VulkanTexture::GetVkImageViewType(TextureType type)
	{
		switch (type)
		{
		case SmolEngine::TextureType::Texture2D:
			return VK_IMAGE_VIEW_TYPE_2D;
		case SmolEngine::TextureType::Texture3D:
			return VK_IMAGE_VIEW_TYPE_3D;
		case SmolEngine::TextureType::CubeMap:
			return VK_IMAGE_VIEW_TYPE_CUBE;
		default:
			return VK_IMAGE_VIEW_TYPE_2D;
		}
	}

	uint32_t VulkanTexture::GetImageArrayLayers(TextureType type)
	{
		switch (type)
		{
		case SmolEngine::TextureType::Texture2D:
			return 1;
		case SmolEngine::TextureType::Texture3D:
			return 4;
		case SmolEngine::TextureType::CubeMap:
			return 6;
		default:
			return 1;
		}
	}
}