#pragma once

#include "Renderer/BufferLayout.h"
#include "Renderer/Vulkan/VulkanSwapchain.h"
#include "Renderer/Vulkan/VulkanShader.h"
#include "Renderer/Vulkan/VulkanDevice.h"
#include "Renderer/Vulkan/VulkanTexture.h"

namespace SmolEngine
{
	struct VulkanPipelineSpecification
	{
		VulkanSwapchain* TargetSwapchain = nullptr;
		BufferLayout* BufferLayout = nullptr;
		VulkanDevice* Device = nullptr;
		VulkanShader* Shader = nullptr;

		std::vector<VulkanTexture*> Textures;
		std::string Name = "";

		bool IsAlphaBlendingEnabled = false;
		bool Initialized = false;

		uint32_t DescriptorSets = 1;
		uint32_t Stride = 0;
	};
}