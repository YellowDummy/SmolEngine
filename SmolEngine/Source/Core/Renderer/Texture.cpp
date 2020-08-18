#include "stdafx.h"
#include "Texture.h"
#include "Core/Core.h"
#include "Core/SLog.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/OpenGL/OpenglTexture.h"

#include <memory>

namespace SmolEngine
{
	Ref<Texture2D> Texture2D::Create(const uint32_t width, const uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: NATIVE_ERROR("No renderer API  specified."); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenglTexture2D>(width, height); break;
		case RendererAPI::API::Vulkan: return nullptr; break;
		default: NATIVE_ERROR("Unknow Renderer API"); return nullptr;
		}
	}

	Ref<Texture2D> Texture2D::Create(const std::string& filePath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: NATIVE_ERROR("No renderer API  specified."); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenglTexture2D>(filePath); break;
		case RendererAPI::API::Vulkan: return nullptr; break;
		default: NATIVE_ERROR("Unknow Renderer API"); return nullptr;
		}
	}
}