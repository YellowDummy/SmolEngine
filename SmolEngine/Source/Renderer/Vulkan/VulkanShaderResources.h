#pragma once
#include "Core/Core.h"

#include <string>
#include <vector>
#include <spirv_cross/spirv_cross.hpp>

#include "Renderer/Vulkan/VulkanBuffer.h"
#include "Renderer/Vulkan/VulkanDescriptor.h"


namespace SmolEngine
{
	struct Uniform
	{
		std::string Name = "";

		size_t Size = 0;
		size_t Offset = 0;

		spirv_cross::SPIRType Type;
	};

	struct UniformBuffer
	{
		std::string Name = "";

		VulkanBuffer VkBuffer = {};
		VkDescriptorBufferInfo DesriptorBufferInfo;
		VkShaderStageFlags StageFlags;

		std::vector<Uniform> Uniforms;

		size_t Size = 0;
		size_t Index = 0;

		uint32_t BindingPoint = 0;
	};

	struct UniformResource
	{
		uint32_t Location = 0;
		uint32_t BindingPoint = 0;
		uint32_t Dimension = 0;
		uint32_t Sampler = 0;
		uint32_t ArraySize = 0;

		VkShaderStageFlags StageFlags;
	};
}