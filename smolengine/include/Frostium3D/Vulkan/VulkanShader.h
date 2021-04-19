#pragma once
#ifndef FROSTIUM_OPENGL_IMPL
#include "Common/Core.h"
#include "Common/GraphicsPipelineInfos.h"

#include "Vulkan/Vulkan.h"
#include "Vulkan/VulkanShaderResources.h"
#include "Vulkan/VulkanDescriptor.h"

#include <string>
#include <unordered_map>
#include <shaderc/shaderc.hpp>

namespace Frostium
{
	class VulkanShader
	{
	public:

		VulkanShader();
		~VulkanShader();

		bool Init(GraphicsPipelineShaderCreateInfo* shaderCI);
		bool Reload();

		// Getters
		static ShaderType GetShaderType(shaderc_shader_kind shadercType);
		static VkShaderStageFlagBits GetVkShaderStage(ShaderType type);
		std::vector<VkPipelineShaderStageCreateInfo>& GetVkPipelineShaderStages();

	private:

		// Compilation
		bool LoadOrCompile(const shaderc::Compiler& compiler, const shaderc::CompileOptions& options, const std::string& filePath,
			shaderc_shader_kind shaderType, bool usePrecompiledBinaries, std::unordered_map<ShaderType, std::vector<uint32_t>>& out_binaryData);
		const shaderc::SpvCompilationResult CompileToSPIRV(const shaderc::Compiler& comp, const shaderc::CompileOptions& options,
			const std::string& source, shaderc_shader_kind type, const std::string& shaderName) const;
		void Reflect(const std::vector<uint32_t>& binaryData, ShaderType shaderType);

		// Helpers
		bool SaveSPIRVBinaries(const std::string& filePath, const std::vector<uint32_t>& data);
		VkShaderModule LoadSPIRVBinaries(const std::string& filePath, ShaderType type);
		const std::string LoadShaderSource(const std::string& filePath);
		void DeleteShaderModules();

	private:

		GraphicsPipelineShaderCreateInfo                    m_Info = {};
		size_t                                              m_MinUboAlignment = 0;

		std::unordered_map<uint32_t, UniformResource>       m_UniformResources;
		std::unordered_map<uint32_t, ShaderBuffer>          m_Buffers;
		std::unordered_map<ShaderType, VkShaderModule>      m_ShaderModules;
		std::vector<VkPushConstantRange>                    m_VkPushConstantRanges;
		std::vector<VkPipelineShaderStageCreateInfo>        m_PipelineShaderStages;

	private:

		friend class VulkanPipeline;
		friend class VulkanPBR;
		friend class VulkanDescriptor;
		friend class GraphicsPipeline;
	};
}
#endif