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

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	struct GraphicsPipelineShaderCreateInfo;

#ifdef  FROSTIUM_OPENGL_IMPL
	class Shader : OpenglShader
#else
	class Shader: VulkanShader
#endif
	{
	public:
		void                                Bind() const;
		void                                UnBind() const;
		bool                                Realod();
#ifndef FROSTIUM_OPENGL_IMPL	                				                
		VulkanShader*                       GetVulkanShader() { return dynamic_cast<VulkanShader*>(this); }
#endif
		static void                         Create(GraphicsPipelineShaderCreateInfo* shaderCI, Shader* shader);

	private:				                
		void                                Reflect(const std::vector<uint32_t>& binaryData, ShaderType type);

	private:
		GraphicsPipelineShaderCreateInfo    m_CreateInfo{};
		ReflectionData                      m_ReflectData{};

		friend class GraphicsPipeline;
	};
}