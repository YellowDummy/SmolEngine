#pragma once
#include "Core/Core.h"
#include "Renderer/ShaderTypes.h"
#include "Renderer/Vulkan/Vulkan.h"
#include "Renderer/Vulkan/VulkanShaderResources.h"
#include "Renderer/Vulkan/VulkanDescriptor.h"

#include <string>
#include <unordered_map>
#include <shaderc/shaderc.hpp>

namespace SmolEngine
{
	class VulkanShader
	{
	public:

		VulkanShader();

		~VulkanShader();

		/// Init
		
		bool Init(const std::string& vertexPath, const std::string& fragmentPath, bool usePrecompiledBinaries = false, bool optimize = false, const std::string& computePath = "");

		bool Reload();

		/// Uniforms
		
		void SetUniformBuffer(size_t bindingPoint, const void* data, size_t size, uint32_t offset = 0);

		/// Getters

		static ShaderType GetShaderType(shaderc_shader_kind shadercType);

		static VkShaderStageFlagBits GetVkShaderStage(ShaderType type);

		const std::vector<VkPipelineShaderStageCreateInfo>& GetVkPipelineShaderStages() const;

	private:

		/// Compilation
		
		bool LoadOrCompile(const shaderc::Compiler& compiler, const shaderc::CompileOptions& options, const std::string& filePath,
			shaderc_shader_kind shaderType, bool usePrecompiledBinaries, std::unordered_map<ShaderType, std::vector<uint32_t>>& out_binaryData);

		const shaderc::SpvCompilationResult CompileToSPIRV(const shaderc::Compiler& comp, const shaderc::CompileOptions& options,
			const std::string& source, shaderc_shader_kind type, const std::string& shaderName) const;

		void Reflect(const std::vector<uint32_t>& binaryData, ShaderType shaderType);

		/// Helpers

		bool SaveSPIRVBinaries(const std::string& filePath, const std::vector<uint32_t>& data);

		VkShaderModule LoadSPIRVBinaries(const std::string& filePath, ShaderType type);

		const std::string LoadShaderSource(const std::string& filePath);

		void DeleteShaderModules();

	private:

		std::vector<VkPushConstantRange>                    m_VkPushConstantRanges;
		std::unordered_map<size_t, UniformResource>         m_UniformResources;
		std::vector<VkPipelineShaderStageCreateInfo>        m_PipelineShaderStages;
		std::unordered_map<size_t, UniformBuffer>           m_UniformBuffers;
		std::unordered_map<ShaderType, std::string>         m_FilePaths;
		std::unordered_map<ShaderType, VkShaderModule>      m_ShaderModules;

		bool                                                m_Optimize = false;
		bool                                                m_IsPrecompiled = false;
		size_t                                              m_MinUboAlignment = 0;

	private:

		friend class VulkanPipeline;
		friend class VulkanPBR;
		friend class VulkanDescriptor;
		friend class GraphicsPipeline;
	};
}