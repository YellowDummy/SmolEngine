#pragma once
#include "Core/Core.h"
#include "Renderer/TextureTypes.h"
#include "Renderer/Vulkan/Vulkan.h"

namespace SmolEngine
{
	class VulkanTexture
	{
	public:

		VulkanTexture();

		~VulkanTexture();

		/// Main
		
		void CreateWhiteTetxure(uint32_t width, uint32_t height);

		void CreateTexture(const std::string& filePath);

		void CreateCubeMap(const std::array<std::string, 6>& filePaths);

		/// Getters
		
		uint32_t GetHeight() const;

		uint32_t GetWidth() const;

		size_t GetID() const;

		void* GetImGuiTextureID() const;

		bool IsActive() const;

	private:

		void CreateTexture(uint32_t width, uint32_t height, void* data, TextureType type);

		void CreateSamplerAndImageView(TextureType type);

		VkImageViewType GetVkImageViewType(TextureType type);

		uint32_t GetImageArrayLayers(TextureType type);

	private:

		VkDescriptorImageInfo m_DescriptorImageInfo;
		VkImage m_Image;

		VkDevice m_Device;
		VkSampler m_Samper;
		VkImageView m_ImageView;
		VkImageLayout m_ImageLayout;
		VkDeviceMemory m_DeviceMemory;

		std::string  m_FilePath = "";

		void* m_ImGuiTextureID = nullptr;
		bool m_IsCreated = false;

		uint32_t m_Height = 0;
		uint32_t m_Width = 0;
		size_t m_ID = 0;

	private:

		friend class VulkanPipeline;
		friend class VulkanDescriptor;
	};
}