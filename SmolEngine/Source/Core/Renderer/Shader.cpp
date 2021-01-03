#include "stdafx.h"
#include "Shader.h"

#include "Core/Renderer/Renderer.h"

#ifdef SMOLENGINE_OPENGL_IMPL

#include "Core/Renderer/OpenGL/OpenglShader.h"

#endif

#include "Core/SLog.h"
#include "Core/ECS/Actor.h"

namespace SmolEngine
{
	void Shader::Create(Ref<Shader>& outShader, const std::string& filePath)
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		outShader->m_OpenglShader.Init(filePath);
#else

#endif
	}

	void Shader::Create(Ref<Shader>& outShader, const std::string& vertexPath, const std::string& fragmentPath, bool optimize, const std::string& computePath)
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		outShader->m_OpenglShader.Init(vertexPath, fragmentPath, vertexPath);
#else
		bool precompiled = false; // temp
		assert(outShader->m_VulkanShader.Init(vertexPath, fragmentPath, precompiled, optimize, computePath) == true);
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

#else
		// Vulkan

#endif

	}

	void Shader::UnBind() const
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglShader.UnBind();

#else
		// Vulkan

#endif

	}

	void Shader::Realod()
	{
#ifdef SMOLENGINE_OPENGL_IMPL


#else
		m_VulkanShader.Reload();

#endif
	}

	void Shader::SetUniformIntArray(const std::string& name, const int* values, uint32_t count)
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglShader.SetUniformIntArray(name, values, count);

#else
		// Vulkan

#endif

	}

	void Shader::SetUniformFloat2(const std::string& name, const glm::vec2& vec2)
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglShader.SetUniformFloat2(name, vec2);

#else
		// Vulkan

#endif

	}

	void Shader::SetUniformFloat3(const std::string& name, const glm::vec3& vec3)
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglShader.SetUniformFloat3(name, vec3);

#else
		// Vulkan

#endif

	}

	void Shader::SetUniformFloat4(const std::string& name, const glm::vec4& vec4)
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglShader.SetUniformFloat4(name, vec4);

#else
		// Vulkan

#endif

	}

	void Shader::SetUniformMat4(const std::string& name, const glm::mat4& mat4)
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglShader.SetUniformMat4(name, mat4);

#else
		// Vulkan

#endif

	}

	void Shader::SetUniformFloat(const std::string& name, const float value)
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglShader.SetUniformFloat(name, value);

#else
		// Vulkan

#endif

	}

	void Shader::SetUniformInt(const std::string& name, const int value)
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglShader.SetUniformInt(name, value);

#else
		// Vulkan

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

	uint32_t Shader::GetProgramID()
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		return m_OpenglShader.GetProgramID();

#else
		// Vulkan

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

	void ShaderLib::AddElement(const Ref<Shader>& element)
	{
		auto name = element->GetName();

		if (!NameExist(name))
		{
			m_ShaderMap[name] = element;
			NATIVE_INFO("Shader Library -> Added new Shader <{}>", name);
		}
		else
		{
			NATIVE_WARN("Shader Library -> Shader <{}> already exist!", name);
		}
	}

	Ref<Shader> ShaderLib::GetElement(const std::string& shaderName)
	{
		return m_ShaderMap[shaderName];
	}

	Ref<Shader> ShaderLib::Load(const std::string& filePath)
	{
		auto shader = std::make_shared<Shader>();
		Shader::Create(shader, filePath);
		AddElement(shader);
		return shader;
	}

	Ref<Shader> ShaderLib::Load(const std::string& vertexPath, const std::string& fragmentPath, bool optimize, const std::string& computePath)
	{
		auto shader = std::make_shared<Shader>();
		Shader::Create(shader, vertexPath, fragmentPath, optimize, computePath);
		AddElement(shader);
		return shader;
	}

	bool ShaderLib::NameExist(const std::string& name)
	{
		return m_ShaderMap.find(name) != m_ShaderMap.end();
	}
}