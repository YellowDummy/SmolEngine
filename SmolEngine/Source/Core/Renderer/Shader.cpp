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
	Ref<Shader> Shader::Create(const std::string& filePath)
	{
		Ref<Shader> shader = std::make_shared<Shader>();

#ifdef SMOLENGINE_OPENGL_IMPL

		shader->m_OpenglShader.Init(filePath);
#else

#endif

		return shader;
	}

	Ref<Shader> Shader::Create(const std::string& vertexSource, const std::string& fragmentSource, const std::string& shaderName, bool optimize)
	{
		Ref<Shader> shader = std::make_shared<Shader>();

#ifdef SMOLENGINE_OPENGL_IMPL

		shader->m_OpenglShader.Init(vertexSource, fragmentSource, shaderName);
#else
		// Vulkan

		const std::string& vertex = LoadShader(vertexSource);
		const std::string& frag = LoadShader(fragmentSource);

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		auto& binaryData = shader->m_BinaryData;

		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);

		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		// Vertex

		const shaderc::SpvCompilationResult result_vertex = shader->CompileToSPIRV(compiler, options, vertex, shaderc_shader_kind::shaderc_vertex_shader, shaderName);

		// Fragment

		const shaderc::SpvCompilationResult result_fragment = shader->CompileToSPIRV(compiler, options, frag, shaderc_shader_kind::shaderc_fragment_shader, shaderName);

		binaryData.emplace((uint32_t)ShaderType::Vertex, std::vector<uint32_t>(result_vertex.cbegin(), result_vertex.cend()));
		binaryData.emplace((uint32_t)ShaderType::Fragment, std::vector<uint32_t>(result_fragment.cbegin(), result_fragment.cend()));

		// Init

		// Link

		// Reflect
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

	void Shader::SumbitUniformBuffer(const std::string& name, const void* data, uint32_t size)
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglShader.SumbitUniformBuffer(name, data, size);

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

	const std::string Shader::LoadShader(const std::string& filePath)
	{
		std::ifstream file(filePath);
		std::stringstream buffer;

		if (!file)
		{
			NATIVE_ERROR("Could not open the file: {}", filePath);
			assert(file);
		}

		buffer << file.rdbuf();
		file.close();

		return buffer.str();
	}

	const shaderc::SpvCompilationResult Shader::CompileToSPIRV(const shaderc::Compiler& comp, const shaderc::CompileOptions& options,
		const std::string& source, shaderc_shader_kind type, const std::string& shaderName) const
	{
		auto result = comp.CompileGlslToSpv(source, type, shaderName.c_str(), options);
		if (result.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			NATIVE_ERROR(result.GetErrorMessage());
			abort();
		}

		return result;
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