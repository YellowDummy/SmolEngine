#include "stdafx.h"
#include "GraphicsContext.h"

#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/Framebuffer.h"

namespace SmolEngine
{
	void GraphicsContext::SetupWindow(GLFWwindow* window, uint32_t* width, uint32_t* height)
	{
		s_Instance = this;
#ifdef  SMOLENGINE_OPENGL_IMPL
		m_OpenglContext.Setup(window);
#else
		m_VulkanContext.Setup(window, width, height);
#endif
	}

	GraphicsContext::~GraphicsContext()
	{
		s_Instance = nullptr;
	}

	bool GraphicsContext::Init(GraphicsContextInitInfo& info)
	{
		if (GraphicsContext::GetSingleton()->m_Initialized)
			return false;

		FramebufferSpecification framebufferCI = {};
		{
			framebufferCI.Width = Application::GetApplication().GetWindowWidth();
			framebufferCI.Height = Application::GetApplication().GetWindowHeight();
			framebufferCI.bUseMSAA = info.bMSAA;
			framebufferCI.bTargetsSwapchain = info.bTargetsSwapchain;
			framebufferCI.bUsedByImGui = true;
			framebufferCI.Attachments = { FramebufferAttachment(AttachmentFormat::Color) };

			GraphicsContext::GetSingleton()->m_Framebuffer = Framebuffer::Create(framebufferCI);
			GraphicsContext::GetSingleton()->m_Initialized = true;
		}

		Renderer::Init();
		Renderer2D::Init();
#ifdef  SMOLENGINE_OPENGL_IMPL
		GraphicsContext::GetSingleton()->GetOpenglRendererAPI()->Init();
#endif
		return true;
	}

	void GraphicsContext::OnResize(uint32_t height, uint32_t width)
	{
#ifdef  SMOLENGINE_OPENGL_IMPL
		m_RendererAPI->SetViewport(0, 0, width, height);
#else
		m_VulkanContext.OnResize(height, width);
#endif
		m_Framebuffer->OnResize(width, height);
	}

	void GraphicsContext::SwapBuffers()
	{
#ifdef  SMOLENGINE_OPENGL_IMPL
		m_OpenglContext.SwapBuffers();
#else
		m_VulkanContext.SwapBuffers();
#endif
	}

	void GraphicsContext::BeginFrame()
	{
#ifdef  SMOLENGINE_OPENGL_IMPL
#else
		m_VulkanContext.BeginFrame();
#endif

	}
}