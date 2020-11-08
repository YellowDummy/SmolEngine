#include "stdafx.h"
#include <fstream>

#include "Core/Renderer/Buffer.h"
#include "OpenglShader.h"
#include "Core/SLog.h"
#include "glad/glad.h"
#include "Core/Renderer/Renderer.h"


namespace SmolEngine
{
	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex") return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;

		NATIVE_ERROR("Unknow shader type");
		abort();
	}

	OpenglShader::OpenglShader(const std::string& filePath)
		:m_RendererID(0), m_Name("Default")
	{
		std::string source = ReadFile(filePath);
		auto shaderSources = PreProcess(source);
		CompileShader(shaderSources);

		auto lastSlash = filePath.find_last_of("\//");
		auto lastDot = filePath.rfind('.');
		auto count = lastDot == std::string::npos ? filePath.size() - lastSlash: lastDot - lastSlash;

		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		m_Name = filePath.substr(lastSlash, count);
		m_Name = m_Name.substr(0, m_Name.size() - 1);
	}

	OpenglShader::OpenglShader(const std::string& vertexSource, const std::string& fragmentSource, const std::string& shaderName)
		:m_Name(shaderName), m_RendererID(-1)
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSource;
		sources[GL_FRAGMENT_SHADER] = fragmentSource;
		CompileShader(sources);
	}

	OpenglShader::~OpenglShader()
	{
		glDeleteProgram(m_RendererID);
	}

	void OpenglShader::CreateUniformMap(const std::vector<std::string>& list)
	{
		for (const auto& pair: list)
		{
			if (m_UniformMap.find(pair) != m_UniformMap.end())
			{
				NATIVE_ERROR("Uniform Map: variable already exists!"); 
				continue;
			}

			GLint location = glGetUniformLocation(m_RendererID, pair.c_str());
			m_UniformMap[pair] = location;
		}
	}

	void OpenglShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void OpenglShader::UnBind() const
	{
		glUseProgram(0);
	}

	void OpenglShader::SetUniformMat4(const std::string& name, const glm::mat4& mat4)
	{
		GLint location = m_UniformMap[name.c_str()];
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat4));
	}

	void OpenglShader::SetUniformFloat3(const std::string& name, const glm::vec3& vec3)
	{
		GLint location = m_UniformMap[name.c_str()];
		glUniform3f(location, vec3.x, vec3.y, vec3.z);
	}

	void OpenglShader::SetUniformFloat4(const std::string& name, const glm::vec4& vec4)
	{
		GLint location = m_UniformMap[name.c_str()];
		glUniform4f(location, vec4.x, vec4.y, vec4.z, vec4.w);
	}

	void OpenglShader::SetUniformInt(const std::string& name, const int value)
	{
		GLint location = m_UniformMap[name.c_str()];
		glUniform1i(location, value);
	}

	void OpenglShader::SetUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = m_UniformMap[name.c_str()];
		glUniform1iv(location, count, values);
	}

	void OpenglShader::SetUniformFloat(const std::string& name, const float value)
	{
		GLint location = m_UniformMap[name.c_str()];
		glUniform1f(location, value);
	}

	void OpenglShader::SetUniformFloat2(const std::string& name, const glm::vec2& float2)
	{
		GLint location = m_UniformMap[name.c_str()];
		glUniform2f(location, float2.x, float2.y);
	}

	void OpenglShader::UploadUniformMatrix3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = m_UniformMap[name.c_str()];
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenglShader::CompileShader(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();
		std::array<GLenum, 2> glShaderIDs;
		if (shaderSources.size() < 2)
		{
			NATIVE_ERROR("Only 2 shaders supported, current size: {}", shaderSources.size()); abort;
		}

		int glShaderIDIndex = 0;
		for (auto& kv : shaderSources)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(type);

			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				std::stringstream ss;

				for (const char c: infoLog)
				{
					ss << c;
				}

				NATIVE_ERROR(ss.str());
				glDeleteShader(shader);
				break;
			}

			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}

		m_RendererID = program;

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			std::stringstream ss;

			for (const char c : infoLog)
			{
				ss << c;
			}

			NATIVE_ERROR(ss.str());

			// We don't need the program anymore.
			glDeleteProgram(program);

			for (auto id : glShaderIDs)
				glDeleteShader(id);

			NATIVE_ERROR("Shader Link Error!");
			return;
		}

		for (auto id : glShaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}
	}

	std::unordered_map<GLenum, std::string> OpenglShader::PreProcess(const std::string& source)
	{

		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
			size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
			std::string type = source.substr(begin, eol - begin);
			ShaderTypeFromString(type);

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
			pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

			shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	std::string OpenglShader::ReadFile(const std::string& file)
	{
		std::string result;
		std::ifstream in(file, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
				in.close();
			}
			else
			{
				NATIVE_ERROR("Could not read from file '{0}'", file);
			}
		}
		else
		{
			NATIVE_ERROR("Could not open file '{0}'", file);
		}

		if (result.empty())
		{
			NATIVE_WARN("File: string is empty");
		}

		return result;
	}

}