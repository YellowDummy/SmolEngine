#pragma once
#include "Common/Core.h"
#include "Common/Common.h"
#include "Common/SPIRVReflection.h"
#ifdef FROSTIUM_OPENGL_IMPL
#include "OpenGL/OpenglShader.h"
#else
#include "Vulkan/VulkanShader.h"
#endif

#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

namespace Frostium
{
	struct GraphicsPipelineShaderCreateInfo;

	class Shader
	{
	public:
		// Factory
		static void Create(GraphicsPipelineShaderCreateInfo* shaderCI, Shader* obj);

		// Getters
		uint32_t GetProgramID();
		const std::string GetName();
#ifdef FROSTIUM_OPENGL_IMPL
#else
		VulkanShader* GetVulkanShader() { return &m_VulkanShader; }
#endif

		void Bind() const;
		void UnBind() const;
		bool Realod();

	private:
		void Reflect(const std::vector<uint32_t>& binaryData, ShaderType shaderType);

	private:
		GraphicsPipelineShaderCreateInfo                        m_CreateInfo{};
		ReflectionData                                          m_ReflectData{};
#ifdef FROSTIUM_OPENGL_IMPL								        
		OpenglShader                                            m_OpenglShader{};
#else													        
		VulkanShader                                            m_VulkanShader{};
#endif 
	};
}