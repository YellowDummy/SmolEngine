#pragma once
#include "Core/Core.h"

#include <string>
#include <vector>
#include <spirv_cross/spirv_cross.hpp>

#include "Renderer/Vulkan/VulkanBuffer.h"
#include "Renderer/Vulkan/VulkanDescriptor.h"


namespace SmolEngine
{
	enum class ShaderBufferType : uint16_t
	{
		Uniform,
		Storage
	};

	struct Uniform
	{
		spirv_cross::SPIRType            Type;
		size_t                           Size = 0;
		size_t                           Offset = 0;
		std::string                      Name = "";
	};

	struct ShaderBuffer
	{
		VkShaderStageFlags               StageFlags;
		ShaderBufferType                 Type;

		uint32_t                         BindingPoint = 0;
		size_t                           Size = 0;
		size_t                           Index = 0;
		std::string                      Name = "";
		std::string                      ObjectName = "";
		std::vector<Uniform>             Uniforms;

	};

	struct UniformResource
	{
		VkShaderStageFlags               StageFlags;
		uint32_t                         Location = 0;
		uint32_t                         BindingPoint = 0;
		uint32_t                         Dimension = 0;
		uint32_t                         Sampler = 0;
		uint32_t                         ArraySize = 0;
	};
}