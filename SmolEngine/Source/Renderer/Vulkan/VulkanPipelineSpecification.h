#pragma once

#include "Renderer/BufferLayout.h"
#include "Renderer/Vulkan/VulkanSwapchain.h"
#include "Renderer/Vulkan/VulkanShader.h"
#include "Renderer/Vulkan/VulkanDevice.h"
#include "Renderer/Vulkan/VulkanTexture.h"

namespace SmolEngine
{
	enum class DrawMode : uint16_t;

	struct VulkanPipelineSpecification
	{
		VulkanSwapchain* TargetSwapchain = nullptr;
		VulkanDevice* Device = nullptr;
		VulkanShader* Shader = nullptr;
		VulkanTexture* Skybox = nullptr;
		BufferLayout BufferLayout = {};

		std::vector<VulkanTexture*> Textures;
		std::vector<DrawMode> PipelineDrawModes;
		std::string Name = "";

		bool IsAlphaBlendingEnabled = false;
		bool IsTargetsSwapchain = false;
		bool Initialized = false; //don't use!

		uint32_t DescriptorSets = 1;
		uint32_t Stride = 0;
		uint32_t Samplers = 10;
	};
}