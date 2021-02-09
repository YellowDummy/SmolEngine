#include "stdafx.h"
#include "Shader.h"

#include "Renderer/GraphicsPipelineShaderCreateInfo.h"

#ifdef SMOLENGINE_OPENGL_IMPL

#include "Renderer/OpenGL/OpenglShader.h"

#endif
#include "Core/SLog.h"
#include "ECS/Actor.h"

namespace SmolEngine
{
	void Shader::Create(Ref<Shader>& outShader, const std::string& filePath)
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		outShader->m_OpenglShader.Init(filePath);
#else
#endif
	}

	void Shader::Create(Ref<Shader>& outShader, GraphicsPipelineShaderCreateInfo* shader)
	{
#ifdef SMOLENGINE_OPENGL_IMPL
#else
		assert(outShader->m_VulkanShader.Init(shader) == true);
#endif
	}

	Shader::Shader()
	{

	}

	Shader::~Shader()
	{

	}

	void Shader::Bind() const
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		m_OpenglShader.Bind();
#endif
	}

	void Shader::UnBind() const
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		m_OpenglShader.UnBind();
#endif
	}

	bool Shader::Realod()
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		return false;
#else
		return m_VulkanShader.Reload();
#endif
	}

	void Shader::SetUniformIntArray(const std::string& name, const int* values, uint32_t count)
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		m_OpenglShader.SetUniformIntArray(name, values, count);
#endif

	}

	void Shader::SetUniformFloat2(const std::string& name, const glm::vec2& vec2)
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		m_OpenglShader.SetUniformFloat2(name, vec2);
#endif

	}

	void Shader::SetUniformFloat3(const std::string& name, const glm::vec3& vec3)
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		m_OpenglShader.SetUniformFloat3(name, vec3);
#endif

	}

	void Shader::SetUniformFloat4(const std::string& name, const glm::vec4& vec4)
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglShader.SetUniformFloat4(name, vec4);
#endif

	}

	void Shader::SetUniformMat4(const std::string& name, const glm::mat4& mat4)
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglShader.SetUniformMat4(name, mat4);

#endif
	}

	void Shader::SetUniformFloat(const std::string& name, const float value)
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglShader.SetUniformFloat(name, value);

#endif
	}

	void Shader::SetUniformInt(const std::string& name, const int value)
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglShader.SetUniformInt(name, value);
#endif
	}

	void Shader::SumbitUniformBuffer(size_t bindPoint, const void* data, size_t size, uint32_t offset)
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		//m_OpenglShader.SumbitUniformBuffer(name, data, size);
#else
		m_VulkanShader.SetUniformBuffer(bindPoint, data, size, offset);

#endif
	}

	void Shader::SumbitStorageBuffer(size_t bindPoint, const void* data, size_t size, uint32_t offset)
	{
#ifdef SMOLENGINE_OPENGL_IMPL
#else
		m_VulkanShader.SetStorageBuffer(bindPoint, data, size, offset);
#endif
	}

	uint32_t Shader::GetProgramID()
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		return m_OpenglShader.GetProgramID();
#else
		return 0;
#endif
	}

	const std::string& Shader::GetName()
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		return m_OpenglShader.GetName();
#else
		// Vulkan
		return std::string("empty");
#endif
	}
}