#pragma once
#include "Core/Core.h"
#include "Core/Renderer/Vulkan/Vulkan.h"
#include "Core/Renderer/ShaderTypes.h"
#include "Core/Renderer/UniformBuffer.h"

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

		/// 
		/// Main
		/// 
		
		bool Init(const std::string& vertexPath, const std::string& fragmentPath, bool usePrecompiledBinaries = false, bool optimize = false, const std::string& computePath = "");

		bool Reload();

		/// 
		/// Uniforms
		/// 
		
		void SetUniformBuffer(const std::string& name, const void* data);

	private:

		/// 
		/// Compilation
		/// 
		
		bool LoadOrCompile(const shaderc::Compiler& compiler, const shaderc::CompileOptions& options, const std::string& filePath,
			shaderc_shader_kind shaderType, bool usePrecompiledBinaries, std::unordered_map<ShaderType, std::vector<uint32_t>>& out_binaryData);

		const shaderc::SpvCompilationResult CompileToSPIRV(const shaderc::Compiler& comp, const shaderc::CompileOptions& options,
			const std::string& source, shaderc_shader_kind type, const std::string& shaderName) const;

		void Reflect(const std::vector<uint32_t>& binaryData);

		/// 
		/// Helpers
		/// 

		bool SaveSPIRVBinaries(const std::string& filePath, const std::vector<uint32_t>& data);

		const std::vector<uint32_t> LoadSPIRVBinaries(const std::string& filePath);

		const std::string LoadShaderSource(const std::string& filePath);

		const std::string GetCachedPath(const std::string& filePath);


		ShaderType GetShaderType(shaderc_shader_kind shadercType);

		VkShaderStageFlagBits GetVkShaderStage(ShaderType type);

		/// 
		/// Getters
		/// 

	public:

		const std::vector<VkPipelineShaderStageCreateInfo>& GetVkPipelineShaderStages();

	private:

		std::unordered_map<std::string, UniformResource> m_UniformResources;
		std::vector<VkPipelineShaderStageCreateInfo> m_PipelineShaderStages;
		std::unordered_map<std::string, UniformBuffer> m_UniformBuffers;
		std::unordered_map<ShaderType, std::string> m_FilePaths;

		bool m_Optimize = false;
		bool m_IsPrecompiled = false;
	};
}