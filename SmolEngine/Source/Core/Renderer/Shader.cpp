#include "stdafx.h"
#include "Shader.h"

#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/OpenGL/OpenglShader.h"
#include "Core/SLog.h"

namespace SmolEngine
{
	Ref<Shader> Shader::Create(const std::string& filePath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: NATIVE_ERROR("No renderer API specified."); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenglShader>(filePath);
		case RendererAPI::API::Vulkan: return nullptr;

		default: NATIVE_ERROR("Unknow Renderer API"); return nullptr;
		}
	}

	Ref<Shader> Shader::Create(const std::string& vertexSource, const std::string& fragmentSource, const std::string& shaderName)
	{
		switch (Renderer::GetAPI()) 
		{
		case RendererAPI::API::None: NATIVE_ERROR("No renderer API specified."); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenglShader>(vertexSource, fragmentSource, shaderName);
		case RendererAPI::API::Vulkan: return nullptr;

		default: NATIVE_ERROR("Unknow Renderer API"); return nullptr;
		}
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