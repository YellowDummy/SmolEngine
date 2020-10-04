#include "stdafx.h"
#include "Text.h"

#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/OpenGL/OpenglText.h"
#include "Core/SLog.h"


namespace SmolEngine
{
	Ref<Text> Text::Create(const std::string& filePath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: NATIVE_ERROR("No renderer API specified."); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenglText>(filePath);
		case RendererAPI::API::Vulkan: return nullptr;

		default: NATIVE_ERROR("Unknow Renderer API"); return nullptr;
		}
	}
}