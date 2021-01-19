#include "stdafx.h"
#include "VulkanTexture.h"
#include "stb_image.h"
#include "stb_image_write.h"

#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanStagingBuffer.h"
#include "Renderer/Vulkan/VulkanMemoryAllocator.h"

#include "../Libraries/imgui/examples/imgui_impl_vulkan.h"

#include <ktx.h>
#include <ktxvulkan.h>

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
		CreateTexture(width, height, 1, &whiteTextureData, TextureType::Texture2D);

		m_Width = width;
		m_Height = height;
		m_IsCreated = true;
	}

	void VulkanTexture::CreateTexture(const std::string& filePath)
	{
		//ktxTexture* ktxTexture;
		//ktxTextureCreateInfo info;

		//auto result = ktxTexture_CreateFromNamedFile(filePath.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);

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

		const uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

		CreateTexture(width, height, mipLevels, data , TextureType::Texture2D);
		m_Width = width;
		m_Height = height;
		m_FilePath = filePath;
		m_IsCreated = true;

		stbi_image_free(data);
		std::hash<std::string> hasher;
		m_ID = hasher(filePath);
	}

	void VulkanTexture::CreateCubeMapKtx(const std::string& filePath)
	{
		ktxResult result;
		ktxTexture* ktxTexture;

		result = ktxTexture_CreateFromNamedFile(filePath.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);
		assert(result == KTX_SUCCESS);

		uint32_t width = ktxTexture->baseWidth;
		uint32_t height = ktxTexture->baseHeight;
		uint32_t mipLevels = ktxTexture->numLevels;

		ktx_uint8_t* ktxTextureData = ktxTexture_GetData(ktxTexture);
		ktx_size_t ktxTextureSize = ktxTexture_GetSize(ktxTexture);

		VulkanBuffer stagingBuffer;
		stagingBuffer.Create(ktxTextureSize, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		stagingBuffer.SetData(ktxTextureData, ktxTextureSize);

		// Create optimal tiled target image
		VkImageCreateInfo imageCreateInfo = {};
		{
			imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
			imageCreateInfo.format = Format;
			imageCreateInfo.mipLevels = mipLevels;
			imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageCreateInfo.extent = { width, height, 1 };
			imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			// Cube faces count as array layers in Vulkan
			imageCreateInfo.arrayLayers = 6;
			// This flag is required for cube map images
			imageCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

			VK_CHECK_RESULT(vkCreateImage(m_Device, &imageCreateInfo, nullptr, &m_Image));
		}

		VkMemoryRequirements memReqs;
		vkGetImageMemoryRequirements(m_Device, m_Image, &memReqs);
		uint32_t typeIndex = VulkanContext::GetDevice().GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VulkanMemoryAllocator::Allocate(m_Device, memReqs, &m_DeviceMemory, typeIndex);
		VK_CHECK_RESULT(vkBindImageMemory(m_Device, m_Image, m_DeviceMemory, 0));

		// Setup buffer copy regions for each face including all of its miplevels
		std::vector<VkBufferImageCopy> bufferCopyRegions;
		uint32_t offset = 0;
		for (uint32_t face = 0; face < 6; face++)
		{
			for (uint32_t level = 0; level < mipLevels; level++)
			{
				// Calculate offset into staging buffer for the current mip level and face
				ktx_size_t offset;
				KTX_error_code ret = ktxTexture_GetImageOffset(ktxTexture, level, 0, face, &offset);
				assert(ret == KTX_SUCCESS);
				VkBufferImageCopy bufferCopyRegion = {};
				bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				bufferCopyRegion.imageSubresource.mipLevel = level;
				bufferCopyRegion.imageSubresource.baseArrayLayer = face;
				bufferCopyRegion.imageSubresource.layerCount = 1;
				bufferCopyRegion.imageExtent.width = ktxTexture->baseWidth >> level;
				bufferCopyRegion.imageExtent.height = ktxTexture->baseHeight >> level;
				bufferCopyRegion.imageExtent.depth = 1;
				bufferCopyRegion.bufferOffset = offset;
				bufferCopyRegions.push_back(bufferCopyRegion);
			}
		}

		// Image barrier for optimal image (target)
		// Set initial layout for all array layers (faces) of the optimal (target) tiled texture
		VkImageSubresourceRange subresourceRange = {};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = mipLevels;
		subresourceRange.layerCount = 6;

		m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkCommandBuffer copyCmd = VulkanContext::GetCommandBuffer().CreateSingleCommandBuffer();
		{
			SetImageLayout(
				copyCmd,
				m_Image,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				subresourceRange);

			vkCmdCopyBufferToImage(
				copyCmd,
				stagingBuffer.GetBuffer(),
				m_Image,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				static_cast<uint32_t>(bufferCopyRegions.size()),
				bufferCopyRegions.data()
			);

			SetImageLayout(
				copyCmd,
				m_Image,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				m_ImageLayout,
				subresourceRange);
		}
		VulkanContext::GetCommandBuffer().EndSingleCommandBuffer(copyCmd);

		// Sampler
		{
			auto& device = VulkanContext::GetDevice();

			// Create sampler
			VkSamplerCreateInfo samplerCI = {};
			samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerCI.maxAnisotropy = 1.0f;
			samplerCI.magFilter = VK_FILTER_LINEAR;
			samplerCI.minFilter = VK_FILTER_LINEAR;
			samplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerCI.addressModeV = samplerCI.addressModeU;
			samplerCI.addressModeW = samplerCI.addressModeU;
			samplerCI.mipLodBias = 0.0f;
			samplerCI.compareOp = VK_COMPARE_OP_NEVER;
			samplerCI.minLod = 0.0f;
			samplerCI.maxLod = mipLevels;
			samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			samplerCI.maxAnisotropy = 1.0f;
			if (device.GetDeviceFeatures()->samplerAnisotropy)
			{
				samplerCI.maxAnisotropy = device.GetDeviceProperties()->limits.maxSamplerAnisotropy;
				samplerCI.anisotropyEnable = VK_TRUE;
			}

			VK_CHECK_RESULT(vkCreateSampler(m_Device, &samplerCI, nullptr, &m_Samper));
		}

		// View
		{
			VkImageViewCreateInfo view = {};
			view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			// Cube map view type
			view.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
			view.format = Format;
			view.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
			view.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
			// 6 array layers (faces)
			view.subresourceRange.layerCount = 6;
			// Set number of mip levels
			view.subresourceRange.levelCount = mipLevels;
			view.image = m_Image;
			VK_CHECK_RESULT(vkCreateImageView(m_Device, &view, nullptr, &m_ImageView));
		}

		m_DescriptorImageInfo = {};
		m_DescriptorImageInfo.imageLayout = m_ImageLayout;
		m_DescriptorImageInfo.imageView = m_ImageView;
		m_DescriptorImageInfo.sampler = m_Samper;

		stagingBuffer.Destroy();
		ktxTexture_Destroy(ktxTexture);
	}

	VkImage VulkanTexture::CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels,
		VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
		VkImageUsageFlags usage, VkDeviceMemory& imageMemory)
	{
		auto device = VulkanContext::GetDevice().GetLogicalDevice();
		VkImage image = VK_NULL_HANDLE;

		VkImageCreateInfo imageCI = {};
		{
			imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCI.imageType = VK_IMAGE_TYPE_2D;
			imageCI.format = format;
			imageCI.mipLevels = mipLevels;
			imageCI.extent.depth = 1;
			imageCI.arrayLayers = 1;
			imageCI.samples = numSamples;
			imageCI.tiling = tiling;
			imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageCI.extent = { (uint32_t)width, (uint32_t)height, 1 };
			imageCI.usage = usage;

			VK_CHECK_RESULT(vkCreateImage(device, &imageCI, nullptr, &image));
		}

		VkMemoryRequirements memReqs;
		vkGetImageMemoryRequirements(device,image, &memReqs);
		uint32_t typeIndex = VulkanContext::GetDevice().GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VulkanMemoryAllocator::Allocate(device, memReqs, &imageMemory, typeIndex);
		VK_CHECK_RESULT(vkBindImageMemory(device, image, imageMemory, 0));

		return image;
	}

	void VulkanTexture::CreateTexture(uint32_t width, uint32_t height, uint32_t mipMaps, void* data, TextureType type)
	{
		const VkDeviceSize size = width * height * 4;

		m_Image = CreateImage(width, height, mipMaps, VK_SAMPLE_COUNT_1_BIT, Format,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, m_DeviceMemory);

		VulkanStagingBuffer stagingBuffer;
		stagingBuffer.Create(size);
		stagingBuffer.SetData(data, size);

		TransitionImageLayout(m_Image,VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipMaps);
		CopyBufferToImage(m_Image, stagingBuffer.GetBuffer(), width, height);
		stagingBuffer.Destroy();

		GenerateMipMaps(m_Image, width, height, mipMaps);
		m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		CreateSamplerAndImageView(type, mipMaps);

#ifdef SMOLENGINE_EDITOR

		m_ImGuiTextureID = ImGui_ImplVulkan_AddTexture(m_DescriptorImageInfo);

#endif // SMOLENGINE_EDITOR
	}

	void VulkanTexture::GenerateMipMaps(VkImage image, int32_t width, int32_t height, uint32_t mipLevel)
	{
		VkCommandBuffer cmdBuffer = VulkanContext::GetCommandBuffer().CreateSingleCommandBuffer();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		for (uint32_t i = 1; i < mipLevel; i++) 
		{
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(cmdBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { width, height, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { width > 1 ? height / 2 : 1, height > 1 ? height / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(cmdBuffer,
				image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(cmdBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			if (width > 1) width /= 2;
			if (height > 1) height /= 2;
		}

		barrier.subresourceRange.baseMipLevel = mipLevel - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(cmdBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		VulkanContext::GetCommandBuffer().EndSingleCommandBuffer(cmdBuffer);
	}

	void VulkanTexture::TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipMaps)
	{
		VkCommandBuffer cmdBuffer = VulkanContext::GetCommandBuffer().CreateSingleCommandBuffer();

		VkImageMemoryBarrier barrier{};
		{
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.image = image;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.levelCount = mipMaps;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;
		}

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else {
			NATIVE_ERROR("unsupported layout transition!");
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			cmdBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		VulkanContext::GetCommandBuffer().EndSingleCommandBuffer(cmdBuffer);
	}

	void VulkanTexture::CopyBufferToImage(VkImage image, VkBuffer buffer, uint32_t width, uint32_t height)
	{
		VkCommandBuffer cmdBuffer = VulkanContext::GetCommandBuffer().CreateSingleCommandBuffer();

		VkBufferImageCopy region{};
		{
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;
			region.imageOffset = { 0, 0, 0 };
			region.imageExtent = {
				width,
				height,
				1
			};
		}

		vkCmdCopyBufferToImage(cmdBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
		VulkanContext::GetCommandBuffer().EndSingleCommandBuffer(cmdBuffer);
	}

	void VulkanTexture::CreateSamplerAndImageView(TextureType type, uint32_t mipMaps)
	{
		/// Samplers
	    /// https://vulkan-tutorial.com/Texture_mapping/Image_view_and_sampler

		VkSamplerCreateInfo samplerCI = {};
		{
			auto& device = VulkanContext::GetDevice();

			samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerCI.maxAnisotropy = 1.0f;
			samplerCI.magFilter = VK_FILTER_LINEAR;
			samplerCI.minFilter = VK_FILTER_LINEAR;
			samplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerCI.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerCI.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerCI.compareOp = VK_COMPARE_OP_NEVER;
			samplerCI.mipLodBias = 0.0f;
			samplerCI.minLod = 0.0f;
			samplerCI.maxLod = static_cast<float>(mipMaps);
			samplerCI.maxAnisotropy = 1.0;
			samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			samplerCI.maxAnisotropy = 1.0f;
			if (device.GetDeviceFeatures()->samplerAnisotropy)
			{
				samplerCI.maxAnisotropy = device.GetDeviceProperties()->limits.maxSamplerAnisotropy;
				samplerCI.anisotropyEnable = VK_TRUE;
			}

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

	void VulkanTexture::SetImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkImageSubresourceRange subresourceRange, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask)
	{
		VkImageMemoryBarrier imageMemoryBarrier = {};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.oldLayout = oldImageLayout;
		imageMemoryBarrier.newLayout = newImageLayout;
		imageMemoryBarrier.image = image;
		imageMemoryBarrier.subresourceRange = subresourceRange;

		// Source layouts (old)
		// Source access mask controls actions that have to be finished on the old layout
		// before it will be transitioned to the new layout
		switch (oldImageLayout)
		{
		case VK_IMAGE_LAYOUT_UNDEFINED:
			// Image layout is undefined (or does not matter)
			// Only valid as initial layout
			// No flags required, listed only for completeness
			imageMemoryBarrier.srcAccessMask = 0;
			break;

		case VK_IMAGE_LAYOUT_PREINITIALIZED:
			// Image is preinitialized
			// Only valid as initial layout for linear images, preserves memory contents
			// Make sure host writes have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			// Image is a color attachment
			// Make sure any writes to the color buffer have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			// Image is a depth/stencil attachment
			// Make sure any writes to the depth/stencil buffer have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			// Image is a transfer source
			// Make sure any reads from the image have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			// Image is a transfer destination
			// Make sure any writes to the image have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			// Image is read by a shader
			// Make sure any shader reads from the image have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		default:
			// Other source layouts aren't handled (yet)
			break;
		}

		// Target layouts (new)
		// Destination access mask controls the dependency for the new image layout
		switch (newImageLayout)
		{
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			// Image will be used as a transfer destination
			// Make sure any writes to the image have been finished
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			// Image will be used as a transfer source
			// Make sure any reads from the image have been finished
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			// Image will be used as a color attachment
			// Make sure any writes to the color buffer have been finished
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			// Image layout will be used as a depth/stencil attachment
			// Make sure any writes to depth/stencil buffer have been finished
			imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			// Image will be read in a shader (sampler, input attachment)
			// Make sure any writes to the image have been finished
			if (imageMemoryBarrier.srcAccessMask == 0)
			{
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			}
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		default:
			// Other source layouts aren't handled (yet)
			break;
		}

		// Put barrier inside setup command buffer
		vkCmdPipelineBarrier(
			cmdbuffer,
			srcStageMask,
			dstStageMask,
			0,
			0, nullptr,
			0, nullptr,
			1, &imageMemoryBarrier);
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

	const VkDescriptorImageInfo& VulkanTexture::GetVkDescriptorImageInfo() const
	{
		return m_DescriptorImageInfo;
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
}