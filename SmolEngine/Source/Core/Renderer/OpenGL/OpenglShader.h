#pragma once
#include "Core/Renderer/Shader.h"
#include <glm/gtc/type_ptr.hpp>

#include <shaderc/shaderc.hpp>

#include "Core/Renderer/UniformBuffer.h"

namespace SmolEngine
{
	typedef unsigned int GLenum;

	class OpenglShader
	{
	public:

		OpenglShader();

		~OpenglShader();

		///
		///  Main
		/// 

		void Init(const shaderc::SpvCompilationResult* vertex, const shaderc::SpvCompilationResult* frag, const shaderc::SpvCompilationResult* compute = nullptr);

		void Init(const std::string& filepath);

		void Init(const std::string& vertexSource, const std::string& fragmentSource, const std::string& shaderName);

	private:

		void Link();

		void Reflect(const std::unordered_map<uint32_t, std::vector<uint32_t>>& binaryData);

	public:

		///
		///  Binding
		/// 

		void Bind() const;

		void UnBind() const;

		///
		///  Uniforms
		/// 

		void CreateUniformMap(const std::vector<std::string>& list);

		void SetUniformIntArray(const std::string& name, const int* values, uint32_t count);

		void SetUniformFloat2(const std::string& name, const glm::vec2& float2);

		void SetUniformFloat3(const std::string& name, const glm::vec3& vec3);

		void SetUniformFloat4(const std::string& name, const glm::vec4& vec4);

		void SetUniformMat4(const std::string& name, const glm::mat4& mat4);

		void SetUniformInt(const std::string& name, const int value);

		void SetUniformFloat(const std::string& name, const float value);

		void UploadUniformMatrix3(const std::string& name, const glm::mat3& matrix);

		void SumbitUniformBuffer(const std::string& name, const void* data, uint32_t size);

		///
		///  Getters
		/// 

		inline const std::string& GetName()  { return m_Name; }

		uint32_t GetProgramID() { return m_RendererID; }

	private:

		void CompileShader(const std::unordered_map<GLenum, std::string>& shaderSources);

		std::unordered_map<GLenum, std::string> OpenglShader::PreProcess(const std::string& source);

	private:

		std::unordered_map<std::string, GLenum> m_UniformMap;
		std::unordered_map<std::string, UniformBuffer> m_UniformBuffers;
		std::unordered_map<std::string, UniformResource> m_UniformResources;

		std::array<int, 3> m_ShaderIDs = {};

		std::string ReadFile(const std::string& file);

		std::string m_Name = "None";

		uint32_t m_RendererID = 0;

	private:

		friend class Shader;
	};
}
