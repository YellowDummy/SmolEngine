#pragma once
#include "Core/Core.h"
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

#include "Core/Renderer/OpenGL/OpenglShader.h"

namespace SmolEngine
{
	class Shader
	{
	public:

		Shader();

		~Shader();

		/// 
		/// Main
		/// 
		

		///
		///  Binding
		/// 

		void Bind() const;

		void UnBind() const;

		///
		///  Uniforms
		/// 

		void SetUniformIntArray(const std::string& name, int* values, uint32_t count);

		void SetUniformFloat2(const std::string& name, const glm::vec2& vec3);

		void SetUniformFloat3(const std::string& name, const glm::vec3& vec3);

		void SetUniformFloat4(const std::string& name, const glm::vec4& vec4);

		void SetUniformMat4(const std::string& name, const glm::mat4& mat4);

		void SetUniformFloat(const std::string& name, const float value);

		void SetUniformInt(const std::string& name, const int value);


		void CreateUniformMap(const std::vector<std::string>& list);

		///
		/// Getters
		/// 

		uint32_t GetProgramID();

		const std::string& GetName();

		///
		/// Factory
		/// 

		static Ref<Shader> Create(const std::string& filePath);

		static Ref<Shader> Create(const std::string& vertexSource, const std::string& fragmentSource, const std::string& shaderName = 0);

	private:

#ifdef SMOLENGINE_OPENGL_IMPL

		OpenglShader m_OpenglShader = {};
#else


#endif // SMOLENGINE_OPENGL_IMPL


	};

	class ShaderLib
	{
	public:

		ShaderLib() = default;

		ShaderLib(const ShaderLib&) = delete;

		///
		/// Main
		/// 

		void AddElement(const Ref<Shader>& element);

		///
		/// Getters
		/// 


		Ref<Shader> GetElement(const std::string& shaderName);

		///
		/// Load
		/// 

		Ref<Shader> Load(const std::string& filePath);

		Ref<Shader> Load(const std::string& vertexSource, const std::string& fragmentSource, const std::string& shaderName);

	private:

		bool NameExist(const std::string& name);

	private:

		std::unordered_map<std::string, Ref<Shader>> m_ShaderMap;

		std::unordered_map<std::string, Ref<Shader>>::iterator it = m_ShaderMap.begin();
	};
}