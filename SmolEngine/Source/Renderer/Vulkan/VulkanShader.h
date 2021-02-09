#pragma once
#include "Core/Core.h"
#include "Renderer/GraphicsPipelineShaderCreateInfo.h"
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
		
		bool Init(GraphicsPipelineShaderCreateInfo* shaderCI);

		bool Reload();

		/// Setters
		
		void SetUniformBuffer(size_t bindingPoint, const void* data, size_t size, uint32_t offset = 0);

		void SetStorageBuffer(size_t bindingPoint, const void* data, size_t size, uint32_t offset = 0);

		/// Getters

		static ShaderType GetShaderType(shaderc_shader_kind shadercType);

		static VkShaderStageFlagBits GetVkShaderStage(ShaderType type);

		std::vector<VkPipelineShaderStageCreateInfo>& GetVkPipelineShaderStages();

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

		GraphicsPipelineShaderCreateInfo                    m_Info = {};
		size_t                                              m_MinUboAlignment = 0;

		std::unordered_map<uint32_t, UniformResource>       m_UniformResources;
		std::unordered_map<uint32_t, UniformBuffer>         m_UniformBuffers;
		std::unordered_map<uint32_t, StorageBuffer>         m_StorageBuffers;
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