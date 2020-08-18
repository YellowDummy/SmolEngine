#pragma once
#include "Core/Renderer/Shader.h"
#include <glm/gtc/type_ptr.hpp>


namespace SmolEngine
{
	typedef unsigned int GLenum;

	enum class ShaderType
	{
		None = 0, Vertex, Fragment
	};


	class OpenglShader: public Shader
	{
	public:
		OpenglShader(const std::string& filepath);
		OpenglShader(const std::string& vertexSource, const std::string& fragmentSource, const std::string& shaderName);

		~OpenglShader();
		void Bind() const override;
		void UnBind() const override;

		void SetUniformInt(const std::string& name, const int value) override;
		virtual void SetUniformIntArray(const std::string& name, int* values, uint32_t count) override;

		void SetUniformFloat(const std::string& name, const float value) override;
		void UploadUniformFloat2(const std::string& name, const glm::vec2 float2);
		void SetUniformFloat3(const std::string& name, const glm::vec3 vec3) override;
		void SetUniformFloat4(const std::string& name, const glm::vec4 vec4) override;

		void SetUniformMat4(const std::string& name, const glm::mat4& mat4) override;
		void UploadUniformMatrix3(const std::string& name, const glm::mat3& matrix);

		inline const std::string& GetName() override { return m_Name; }
	private:
		std::unordered_map<GLenum, std::string> OpenglShader::PreProcess(const std::string& source);
		void CompileShader(const std::unordered_map<GLenum, std::string>& shaderSources);
		std::string ReadFile(const std::string& file);
	private:
		std::string m_Name;
		uint32_t m_RendererID;
	};
}
