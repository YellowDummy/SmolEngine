#pragma once
#ifndef FROSTIUM_OPENGL_IMPL
#include "Vulkan/Vulkan.h"

#include "Common/Core.h"
#include "Common/Common.h"

namespace Frostium
{
	class VulkanTexture
	{
	public:

		VulkanTexture();
		~VulkanTexture();

		void LoadTexture(const std::string& filePath, bool flip, TextureFormat format);
		void LoadCubeMap(const std::string& filePath, TextureFormat format);
		void GenTexture(const void* data, uint32_t size, uint32_t width, uint32_t height, TextureFormat format);
		void GenWhiteTetxure(uint32_t width, uint32_t height);

		// Getters
		const VkDescriptorImageInfo& GetVkDescriptorImageInfo() const;
		void* GetImGuiTextureID() const;
		uint32_t GetHeight() const;
		uint32_t GetWidth() const;
		bool IsActive() const;
		size_t GetID() const;

		static VkImage CreateVkImage(uint32_t width, uint32_t height, int32_t mipLevels,
			VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
			VkImageUsageFlags usage,
			VkDeviceMemory& imageMemory, uint32_t arrayLayers = 1);

		static void InsertImageMemoryBarrier(
			VkCommandBuffer cmdbuffer,
			VkImage image,
			VkAccessFlags srcAccessMask,
			VkAccessFlags dstAccessMask,
			VkImageLayout oldImageLayout,
			VkImageLayout newImageLayout,
			VkPipelineStageFlags srcStageMask,
			VkPipelineStageFlags dstStageMask,
			VkImageSubresourceRange subresourceRange);

		static void SetImageLayout(VkCommandBuffer cmdbuffer,
			VkImage image,
			VkImageLayout oldImageLayout,
			VkImageLayout newImageLayout,
			VkImageSubresourceRange subresourceRange,
			VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

		static void SetImageLayout(
			VkCommandBuffer cmdbuffer,
			VkImage image,
			VkImageAspectFlags aspectMask,
			VkImageLayout oldImageLayout,
			VkImageLayout newImageLayout,
			VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

	private:

		void GenerateMipMaps(VkImage image, int32_t width, int32_t height, int32_t mipMaps, VkImageSubresourceRange& range);
		void CreateTexture(uint32_t width, uint32_t height, uint32_t mipMaps, const void* data);
		void CreateFromBuffer(const void* data, VkDeviceSize size, uint32_t width, uint32_t height);
		void CreateSamplerAndImageView(uint32_t mipMaps);
		VkFormat GetImageFormat(TextureFormat format);

	private:

		VkDescriptorImageInfo        m_DescriptorImageInfo;
		VkImage                      m_Image;
		VkFormat                     m_Format;

		VkDevice                     m_Device;
		VkSampler                    m_Samper;
		VkImageView                  m_ImageView;
		VkImageLayout                m_ImageLayout;
		VkDeviceMemory               m_DeviceMemory;

		void* m_ImGuiTextureID = nullptr;
		bool                        m_IsCreated = false;

		uint32_t                    m_Height = 0;
		uint32_t                    m_Width = 0;
		size_t                      m_ID = 0;

		std::string                 m_FilePath = "";

	private:

		friend class VulkanPipeline;
		friend class VulkanPBR;
		friend class VulkanDescriptor;
	};
}
#endif