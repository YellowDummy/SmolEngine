#include "stdafx.h"
#include "Buffer.h"

#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/OpenGL/OpenglBuffer.h"
#include "Core/Renderer/OpenGL/OpenglVertexArray.h"
#include "Core/SLog.h"


namespace SmolEngine
{
	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: NATIVE_ERROR("No renderer API specified."); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenglVertexBuffer>(size);
		case RendererAPI::API::Vulkan: return nullptr;

		default: NATIVE_ERROR("Unknow Renderer API"); return nullptr;
		}
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: NATIVE_ERROR("No renderer API specified."); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenglVertexBuffer>(vertices, size);
		case RendererAPI::API::Vulkan: return nullptr;

		default: NATIVE_ERROR("Unknow Renderer API"); return nullptr;
		}
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: NATIVE_ERROR("No renderer API specified."); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenglIndexBuffer>(indices, count);
		case RendererAPI::API::Vulkan: return nullptr;

		default: NATIVE_ERROR("Unknow Renderer API"); return nullptr;
		}
	}

	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: NATIVE_ERROR("No renderer API specified."); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenglVertexArray>();
		case RendererAPI::API::Vulkan: return nullptr;

		default: NATIVE_ERROR("Unknow Renderer API"); return nullptr;
		}
	}
}