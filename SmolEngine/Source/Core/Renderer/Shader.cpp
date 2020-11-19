#include "stdafx.h"
#include "Shader.h"

#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/OpenGL/OpenglShader.h"
#include "Core/SLog.h"

#include <shaderc/shaderc.hpp>

namespace SmolEngine
{
	Ref<Shader> Shader::Create(const std::string& filePath)
	{
		const bool optimize = true;
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;



		Ref<Shader> shader = std::make_shared<Shader>();

#ifdef SMOLENGINE_OPENGL_IMPL

		shader->m_OpenglShader.Init(filePath);
#else

		shader = nullptr;

#endif

		return shader;
	}

	Ref<Shader> Shader::Create(const std::string& vertexSource, const std::string& fragmentSource, const std::string& shaderName)
	{
		Ref<Shader> shader = std::make_shared<Shader>();

#ifdef SMOLENGINE_OPENGL_IMPL

		shader->m_OpenglShader.Init(vertexSource, fragmentSource, shaderName);
#else

		shader = nullptr;

#endif

		return shader;
	}

	Shader::Shader()
	{

	}

	Shader::~Shader()
	{

	}

	void Shader::CreateUniformMap(const std::vector<std::string>& list)
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglShader.CreateUniformMap(list);

#else
		// Vulkan

#endif

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

	void Shader::SetUniformIntArray(const std::string& name, int* values, uint32_t count)
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

	uint32_t Shader::GetProgramID()
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		return m_OpenglShader.GetProgramID();

#else
		// Vulkan

#endif

	}

	const std::string& Shader::GetName()
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		return m_OpenglShader.GetName();

#else
		// Vulkan

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
		auto shader = Shader::Create(filePath);
		AddElement(shader);
		return shader;
	}

	Ref<Shader> ShaderLib::Load(const std::string& vertexSource, const std::string& fragmentSource, const std::string& shaderName)
	{
		auto shader = Shader::Create(vertexSource, fragmentSource, shaderName);
		AddElement(shader);
		return shader;
	}

	bool ShaderLib::NameExist(const std::string& name)
	{
		return m_ShaderMap.find(name) != m_ShaderMap.end();
	}
}