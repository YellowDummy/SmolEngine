#pragma once
#include "Core/Renderer/Vulkan/VulkanSwapchain.h"
#include "Core/Renderer/Vulkan/VulkanShader.h"
#include "Core/Renderer/Vulkan/VulkanDevice.h"
#include "Core/Renderer/Vulkan/VulkanTexture.h"

namespace SmolEngine
{
	struct VulkanPipelineSpecification
	{
		VulkanSwapchain* TargetSwapchain = nullptr;
		VulkanDevice* Device = nullptr;
		VulkanShader* Shader = nullptr;

		VulkanTexture* Texture = nullptr;
	};
}