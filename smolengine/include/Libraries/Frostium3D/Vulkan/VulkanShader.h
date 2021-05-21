#pragma once
#ifndef FROSTIUM_OPENGL_IMPL
#include "Common/Core.h"
#include "Common/SPIRVReflection.h"
#include "Common/GraphicsPipelineInfos.h"

#include "Vulkan/Vulkan.h"
#include "Vulkan/VulkanDescriptor.h"

#include <string>
#include <unordered_map>
#include <shaderc/shaderc.hpp>

namespace Frostium
{
	struct GraphicsPipelineShaderCreateInfo;

	class VulkanShader
	{
	public:

		VulkanShader();
		~VulkanShader();
														         
		bool                                                     Init(std::unordered_map<ShaderType, std::vector<uint32_t>>& binary, ReflectionData* reflectData, GraphicsPipelineShaderCreateInfo* createInfo);
		void                                                     Clean();
														         
		std::vector<VkPipelineShaderStageCreateInfo>&            GetVkPipelineShaderStages();
		static VkShaderStageFlagBits                             GetVkShaderStage(ShaderType type);
														         
	private:											         
		void                                                     DeleteShaderModules();

	private:
		ReflectionData*                                          m_ReflectionData = nullptr;
		GraphicsPipelineShaderCreateInfo*                        m_CreateInfo = nullptr;
		std::unordered_map<ShaderType, VkShaderModule>           m_ShaderModules;
		std::vector<VkPushConstantRange>                         m_VkPushConstantRanges;
		std::vector<VkPipelineShaderStageCreateInfo>             m_PipelineShaderStages;

	private:

		friend class VulkanPipeline;
		friend class VulkanPBR;
		friend class VulkanDescriptor;
		friend class GraphicsPipeline;
	};
}
#endif