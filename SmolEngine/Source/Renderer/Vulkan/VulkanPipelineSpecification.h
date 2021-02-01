#pragma once

#include "Renderer/SharedUtils.h"
#include "Renderer/Vulkan/VulkanSwapchain.h"
#include "Renderer/Vulkan/VulkanShader.h"
#include "Renderer/Vulkan/VulkanDevice.h"
#include "Renderer/Vulkan/VulkanTexture.h"


namespace SmolEngine
{
	enum class DrawMode : uint16_t;

	struct VulkanPipelineSpecification
	{
		VulkanSwapchain*                TargetSwapchain = nullptr;
		VulkanDevice*                   Device = nullptr;
		VulkanShader*                   Shader = nullptr;

		std::vector<VertexInputInfo>    VertexInputInfos;
		std::vector<DrawMode>           PipelineDrawModes;
		std::string                     Name = "";

		bool                            IsAlphaBlendingEnabled = false;
		bool                            IsTargetsSwapchain = false;
		bool                            IsDepthTestEnabled = true;
		bool                            IsUseMRT = false;
		bool                            Initialized = false; //don't use!

		uint32_t                        DescriptorSets = 1;
		uint32_t                        Samplers = 10;
	};
}