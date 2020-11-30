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

	OpenglShader::OpenglShader()
	{
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

	void OpenglShader::Init(const std::string& filePath)
	{
		std::string source = ReadFile(filePath);
		auto shaderSources = PreProcess(source);
		CompileShader(shaderSources);

		auto lastSlash = filePath.find_last_of("\//");
		auto lastDot = filePath.rfind('.');
		auto count = lastDot == std::string::npos ? filePath.size() - lastSlash : lastDot - lastSlash;

		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		m_Name = filePath.substr(lastSlash, count);
		m_Name = m_Name.substr(0, m_Name.size() - 1);
	}

	void OpenglShader::Init(const std::string& vertexSource, const std::string& fragmentSource, const std::string& shaderName)
	{
		m_Name = shaderName;

		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSource;
		sources[GL_FRAGMENT_SHADER] = fragmentSource;
		CompileShader(sources);
	}

	void OpenglShader::Init(const shaderc::SpvCompilationResult* vertex, const shaderc::SpvCompilationResult* frag, const shaderc::SpvCompilationResult* compute)
	{
		m_RendererID = glCreateProgram();

		NATIVE_ERROR(m_RendererID);

		//Vertex
		{
			const uint8_t* begin = (const uint8_t*)vertex->cbegin();
			const uint8_t* end = (const uint8_t*)vertex->cend();
			const ptrdiff_t size = end - begin; // note: pointer can have a negative value

			GLuint shaderID = glCreateShader(GL_VERTEX_SHADER);
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, begin, size);
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(m_RendererID, shaderID);

			m_ShaderIDs[0] = shaderID;
		}

		// Fragment
		{
			const uint8_t* begin = (const uint8_t*)frag->cbegin();
			const uint8_t* end = (const uint8_t*)frag->cend();
			const ptrdiff_t size = end - begin; // note: pointer can have a negative value

			GLuint shaderID = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, begin, size);
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(m_RendererID, shaderID);

			m_ShaderIDs[1] = shaderID;
		}

		if (compute == nullptr)
		{
			return;
		}

		// Compute
		{
			const uint8_t* begin = (const uint8_t*)compute->cbegin();
			const uint8_t* end = (const uint8_t*)compute->cend();
			const ptrdiff_t size = end - begin; // note: pointer can have a negative value

			GLuint shaderID = glCreateShader(GL_COMPUTE_SHADER);
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, begin, size);
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(m_RendererID, shaderID);

			m_ShaderIDs[2] = shaderID;
		}
	}


	void OpenglShader::Link()
	{
		// Link our program

		glLinkProgram(m_RendererID);

		// Note the different functions here: glGetProgram* instead of glGetShader*

		GLint isLinked = 0;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);

			std::stringstream ss;

			for (const char c : infoLog)
			{
				ss << c;
			}

			NATIVE_ERROR(ss.str());

			// We don't need the program anymore

			glDeleteProgram(m_RendererID);

			for (auto id : m_ShaderIDs)
				glDeleteShader(id);

			NATIVE_ERROR("Shader Link Error!");
		}

	}

	void OpenglShader::Reflect(const std::unordered_map<uint32_t, std::vector<uint32_t>>& binaryData)
	{
		glUseProgram(m_RendererID);

		for (const auto& info : binaryData)
		{
			const auto& [key, data] = info;

			spirv_cross::Compiler compiler(data);
			spirv_cross::ShaderResources resources = compiler.get_shader_resources();

			uint32_t bufferIndex = 0;
			for (const auto& res : resources.uniform_buffers)
			{
				auto& type = compiler.get_type(res.base_type_id);
				int bufferElements = type.member_types.size();

				UniformBuffer buffer = {};
				{
					buffer.BindingPoint = compiler.get_decoration(res.id, spv::DecorationBinding);
					buffer.Size = compiler.get_declared_struct_size(type);
					buffer.Index = bufferIndex;

					buffer.Uniforms.reserve(bufferElements);
				}

				for (uint32_t i = 0; i < bufferElements; ++i)
				{
					Uniform uniform = {};
					{
						uniform.Name = compiler.get_member_name(type.self, i);
						uniform.Type = compiler.get_type(type.member_types[i]);
						uniform.Size = compiler.get_declared_struct_member_size(type, i);
						uniform.Offset = compiler.type_struct_member_offset(type, i);
					}

					buffer.Uniforms.push_back(uniform);
				}

				glCreateBuffers(1, &buffer.RendererID);
				glBindBuffer(GL_UNIFORM_BUFFER, buffer.RendererID);
				glBufferData(GL_UNIFORM_BUFFER, buffer.Size, nullptr, GL_DYNAMIC_DRAW);
				glBindBufferBase(GL_UNIFORM_BUFFER, buffer.BindingPoint, buffer.RendererID);

				m_UniformBuffers[res.name] = std::move(buffer);

				bufferIndex++;
			}

			int32_t sampler = 0;
			for (const auto& res : resources.sampled_images)
			{
				UniformResource resBuffer = {};
				{
					resBuffer.Type = compiler.get_type(res.type_id);
					resBuffer.Location = compiler.get_decoration(res.id, spv::DecorationLocation);
					resBuffer.Dimension = resBuffer.Type.image.dim;
					resBuffer.Sampler = sampler;
					resBuffer.ArraySize = resBuffer.Type.array[0];
				}

				int32_t samplers[LayerDataBuffer::MaxTextureSlot];
				for (uint32_t i = 0; i < LayerDataBuffer::MaxTextureSlot; i++)
				{
					samplers[i] = i;
				}

				glUniform1iv(resBuffer.Location, LayerDataBuffer::MaxTextureSlot, samplers);

				m_UniformResources[res.name] = std::move(resBuffer);
				sampler++;

			}
		}

		// Temp

		for (const auto& info : m_UniformBuffers)
		{
			const auto& [name, res] = info;
			NATIVE_WARN("UniformBuffer\nMembers: {}, Name: {}, Binding: {}", res.Uniforms.size(), name, res.BindingPoint);
		}

		for (const auto& info : m_UniformResources)
		{
			const auto& [name, res] = info;
			NATIVE_WARN("UniformResource\nSampler: {}, Location: {}, Dim: {}, Name: {}", res.Sampler, res.Location, res.Dimension, name);
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

	void OpenglShader::SetUniformIntArray(const std::string& name, const int* values, uint32_t count)
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

	void OpenglShader::SumbitUniformBuffer(const std::string& name, const void* data, uint32_t size)
	{
		const auto& result = m_UniformBuffers.find(name);
		if (result != m_UniformBuffers.end())
		{
			uint8_t* buffer = new uint8_t[size];
			memcpy(buffer, data, size);

			glNamedBufferSubData(result->second.RendererID, 0, size, buffer);
			delete[] buffer;
		}
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