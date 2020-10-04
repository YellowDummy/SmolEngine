#pragma once
#include "Core/Core.h"
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

namespace SmolEngine
{
	class Shader
	{
	public:

		virtual ~Shader() = default;

		virtual void Bind() const = 0;

		virtual void UnBind() const = 0;

		virtual void SetUniformMat4(const std::string& name, const glm::mat4& mat4) = 0;

		virtual void SetUniformFloat(const std::string& name, const float value) = 0;

		virtual void SetUniformFloat2(const std::string& name, const glm::vec2& vec3) = 0;

		virtual void SetUniformFloat3(const std::string& name, const glm::vec3& vec3) = 0;

		virtual void SetUniformFloat4(const std::string& name, const glm::vec4& vec4) = 0;

		virtual void SetUniformInt(const std::string& name, const int value) = 0;

		virtual void SetUniformIntArray(const std::string& name, int* values, uint32_t count) = 0;

		virtual uint32_t GetProgramID() = 0;

		virtual const std::string& GetName() { return std::string(""); }

		static Ref<Shader> Create(const std::string& filePath);

		static Ref<Shader> Create(const std::string& vertexSource, const std::string& fragmentSource, const std::string& shaderName = 0);

	private:

	};

	class ShaderLib
	{
	public:

		ShaderLib() = default;

		ShaderLib(const ShaderLib&) = delete;

		void AddElement(const Ref<Shader>& element);

		Ref<Shader> GetElement(const std::string& shaderName);

		Ref<Shader> Load(const std::string& filePath);

		Ref<Shader> Load(const std::string& vertexSource, const std::string& fragmentSource, const std::string& shaderName);

	private:

		bool NameExist(const std::string& name);

	private:

		std::unordered_map<std::string, Ref<Shader>> m_ShaderMap;
		std::unordered_map<std::string, Ref<Shader>>::iterator it = m_ShaderMap.begin();
	};
}