#pragma once
#include "Core/Core.h"
#include "Core/Renderer/Vulkan/Vulkan.h"

namespace SmolEngine
{
	class VulkanTexture
	{
	public:

		VulkanTexture();

		~VulkanTexture();

		/// 
		/// Main
		/// 
		
		void CreateTexture2D(uint32_t width, uint32_t height);

		void CreateTexture2D(const std::string& filePath);

		void CreateCubeTexture();

		///
		/// Getters
		///
		
		uint32_t GetHeight() const;

		uint32_t GetWidth() const;

		bool IsActive() const;


		VkImage m_Image;

		VkDevice m_Device;
		VkSampler m_Samper;
		VkImageView m_ImageView;
		VkImageLayout m_ImageLayout;

	private:

		VkDeviceMemory m_DeviceMemory;

		uint32_t m_Height = 0;
		uint32_t m_Width = 0;

		std::string  m_FilePath = "";

		bool m_IsCreated = false;
	};
}