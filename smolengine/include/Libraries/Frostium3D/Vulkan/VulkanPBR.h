#pragma once
#ifndef FROSTIUM_OPENGL_IMPL
#include "Common/Core.h"
#include "Common/Common.h"

#include "Vulkan/Vulkan.h"

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	struct PBRAttachment
	{
		VkImage Image = nullptr;
		VkImageView ImageView = nullptr;
		VkSampler Sampler = nullptr;
		VkDeviceMemory DeviceMemory = nullptr;
	};

	class VulkanTexture;
	class CubeMap;

	class VulkanPBR
	{
	public:
		VulkanPBR();
		void GeneratePBRCubeMaps(CubeMap* environment_map);
		void Free();

		// Getters
		static VulkanPBR* GetSingleton();
		const VkDescriptorImageInfo& GetBRDFLUTImageInfo();
		const VkDescriptorImageInfo& GetIrradianceImageInfo();
		const VkDescriptorImageInfo& GetPrefilteredCubeImageInfo();

	private:

		void GenerateBRDFLUT(VkImage outImage, VkImageView outImageView,
			VkSampler outSampler, VkDeviceMemory outImageMem,
			VkDescriptorImageInfo& outImageInfo);

		void GenerateIrradianceCube(VkImage outImage, VkImageView outImageView,
			VkSampler outSampler, VkDeviceMemory outImageMem, VulkanTexture* skyBox,
			VkDescriptorImageInfo& outImageInfo);

		void GeneratePrefilteredCube(VkImage outImage, VkImageView outImageView,
			VkSampler outSampler, VkDeviceMemory outImageMem, VulkanTexture* skyBox,
			VkDescriptorImageInfo& outImageInfo);

		void DestroyAttachment(const  PBRAttachment& obj);

	private:

		inline static VulkanPBR*              s_Instance = nullptr;

		PBRAttachment                         m_BRDFLUT = {};
		PBRAttachment                         m_Irradiance = {};
		PBRAttachment                         m_PrefilteredCube = {};
		VkDescriptorImageInfo                 m_BRDFLUTImageInfo = {};
		VkDescriptorImageInfo                 m_IrradianceImageInfo = { };
		VkDescriptorImageInfo                 m_PrefilteredCubeImageInfo = {};
	};
}
#endif