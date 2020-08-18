#include "stdafx.h"
#include "Framebuffer.h"
#include "Core/Renderer/OpenGL/OpenglFramebuffer.h"
#include "Core/Renderer/Renderer.h"
#include "Core/SLog.h"

namespace SmolEngine
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferData& data)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: NATIVE_ERROR("No renderer API specified."); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenglFramebuffer>(data);
		case RendererAPI::API::Vulkan: return nullptr;

		default: NATIVE_ERROR("Unknow Renderer API"); return nullptr;
		}
	}
}