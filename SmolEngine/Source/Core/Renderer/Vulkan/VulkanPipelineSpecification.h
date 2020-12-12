#pragma once
#include "Core/Renderer/BufferLayout.h"
#include "Core/Renderer/Vulkan/VulkanSwapchain.h"
#include "Core/Renderer/Vulkan/VulkanShader.h"
#include "Core/Renderer/Vulkan/VulkanDevice.h"
#include "Core/Renderer/Vulkan/VulkanTexture.h"

namespace SmolEngine
{
	struct VulkanPipelineSpecification
	{
		VulkanSwapchain* TargetSwapchain = nullptr;
		BufferLayout* BufferLayout = nullptr;
		VulkanDevice* Device = nullptr;
		VulkanShader* Shader = nullptr;

		std::vector<VulkanTexture*> Textures;
	};
}