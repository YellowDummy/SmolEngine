#pragma once
#include "Core/Core.h"
#include "Renderer/Vulkan/Vulkan.h"
#include "Renderer/TextureFormat.h"

namespace SmolEngine
{
	struct PBRAttachment
	{
		VkImage Image = nullptr;
		VkImageView ImageView = nullptr;
		VkSampler Sampler = nullptr;
		VkDeviceMemory DeviceMemory = nullptr;
	};

	class VulkanTexture;

	class VulkanPBR
	{
	public:

		static void Init(const std::string& cubeMapFile, TextureFormat format = TextureFormat::R8G8B8A8_UNORM);

		static void Reload(const std::string& cubeMapFile, TextureFormat format = TextureFormat::R8G8B8A8_UNORM);

		static void Free();

		// Getters

		static const VkDescriptorImageInfo& GetBRDFLUTImageInfo();

		static const VkDescriptorImageInfo& GetIrradianceImageInfo();

		static const VkDescriptorImageInfo& GetPrefilteredCubeImageInfo();

		static const VulkanTexture&  GetSkyBox();

	private:

		static void GenerateBRDFLUT(VkImage outImage, VkImageView outImageView,
			VkSampler outSampler, VkDeviceMemory outImageMem,
			VkDescriptorImageInfo& outImageInfo);

		static void GenerateIrradianceCube(VkImage outImage, VkImageView outImageView,
			VkSampler outSampler, VkDeviceMemory outImageMem, VulkanTexture* skyBox,
			VkDescriptorImageInfo& outImageInfo);

		static void GeneratePrefilteredCube(VkImage outImage, VkImageView outImageView,
			VkSampler outSampler, VkDeviceMemory outImageMem, VulkanTexture* skyBox,
			VkDescriptorImageInfo& outImageInfo);

		static void DestroyAttachment(const  PBRAttachment& obj);

	private:

		inline static PBRAttachment                         m_BRDFLUT = {};
		inline static PBRAttachment                         m_Irradiance = {};
		inline static PBRAttachment                         m_PrefilteredCube = {};

		inline static VkDescriptorImageInfo                 m_BRDFLUTImageInfo = {};
		inline static VkDescriptorImageInfo                 m_IrradianceImageInfo = { };
		inline static VkDescriptorImageInfo                 m_PrefilteredCubeImageInfo = {};

		inline static VulkanTexture*                        m_SkyBox = nullptr;
	};
}