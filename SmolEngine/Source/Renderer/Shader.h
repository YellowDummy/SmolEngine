#pragma once
#include "Core/Core.h"

#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

#include <shaderc/shaderc.hpp>

#ifdef SMOLENGINE_OPENGL_IMPL
#include "Renderer/OpenGL/OpenglShader.h"
#else
#include "Renderer/Vulkan/VulkanShader.h"
#endif

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
		
		void Bind() const;

		void UnBind() const;

		void Realod();

		///
		///  Uniforms
		/// 

		void SumbitUniformBuffer(size_t bindPoint, const void* data, size_t size, uint32_t offset = 0);

		template<typename T>
		void SumbitUniform(const std::string& name, const void* data, uint32_t count = 0, uint32_t size = 0)
		{

#ifdef SMOLENGINE_OPENGL_IMPL

			if (std::is_same<T, float>::value)
			{
				SetUniformFloat(name, *static_cast<const float*>(data));
			}

			if (std::is_same<T, glm::vec2>::value)
			{
				SetUniformFloat2(name, *static_cast<const glm::vec2*>(data));
			}

			if (std::is_same<T, glm::vec3>::value)
			{
				SetUniformFloat3(name, *static_cast<const glm::vec3*>(data));
			}

			if (std::is_same<T, glm::vec4>::value)
			{
				SetUniformFloat4(name, *static_cast<const glm::vec4*>(data));
			}

			if (std::is_same<T, glm::mat4>::value)
			{
				SetUniformMat4(name, *static_cast<const glm::mat4*>(data));
			}

			if (std::is_same<T, int*>::value)
			{
				if (count == 0)
				{
					NATIVE_ERROR("Pushing UniformArray: count is 0");
					abort();
				}

				SetUniformIntArray(name, static_cast<const int*>(data), count);
			}

			if (std::is_same<T, int>::value)
			{
				SetUniformInt(name, *static_cast<const int*>(data));
			}
#endif
		}

	private:

		void SetUniformIntArray(const std::string& name, const int* values, uint32_t count);

		void SetUniformFloat2(const std::string& name, const glm::vec2& vec3);

		void SetUniformFloat3(const std::string& name, const glm::vec3& vec3);

		void SetUniformFloat4(const std::string& name, const glm::vec4& vec4);

		void SetUniformMat4(const std::string& name, const glm::mat4& mat4);

		void SetUniformFloat(const std::string& name, const float value);

		void SetUniformInt(const std::string& name, const int value);

	public:

		///
		/// Getters
		/// 

		uint32_t GetProgramID();

		const std::string& GetName();

		/// 
		/// Operators
		/// 

#ifdef SMOLENGINE_OPENGL_IMPL

		operator OpenglShader() const { return m_OpenglShader; }
#else
		operator  VulkanShader() const { return m_VulkanShader; }
#endif

#ifdef SMOLENGINE_OPENGL_IMPL

#else
		VulkanShader* GetVulkanShader() { return &m_VulkanShader; }
#endif

		///
		/// Factory
		/// 

		static void Create(Ref<Shader>& outShader, const std::string& filePath);

		static void Create(Ref<Shader>& outShader, const std::string& vertexPath, const std::string& fragmentPath, bool optimize = false, const std::string& computePath = "");

	private:

#ifdef SMOLENGINE_OPENGL_IMPL

		OpenglShader m_OpenglShader = {};
#else
		VulkanShader m_VulkanShader = {};

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

		Ref<Shader> Load(const std::string& vertexPath, const std::string& fragmentPath, bool optimize = false, const std::string& computePath = "");

	private:

		bool NameExist(const std::string& name);

	private:

		std::unordered_map<std::string, Ref<Shader>> m_ShaderMap;

		std::unordered_map<std::string, Ref<Shader>>::iterator it = m_ShaderMap.begin();
	};
}