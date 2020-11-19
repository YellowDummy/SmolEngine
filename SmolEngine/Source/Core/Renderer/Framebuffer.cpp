#include "stdafx.h"
#include "Framebuffer.h"
#include "Core/Renderer/OpenGL/OpenglFramebuffer.h"
#include "Core/Renderer/Renderer.h"
#include "Core/SLog.h"

namespace SmolEngine
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferData& data)
	{
		Ref<Framebuffer> framebuffer = std::make_shared<Framebuffer>();

#ifdef SMOLENGINE_OPENGL_IMPL

		framebuffer->m_OpenglFramebuffer.Init(data);
#else

		// Vulkan

		return nullptr;
#endif

		return framebuffer;
	}

	void Framebuffer::Bind()
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglFramebuffer.Bind();
#else

#endif

	}

	void Framebuffer::BindColorAttachment(uint32_t slot)
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglFramebuffer.BindColorAttachment(slot);
#else

#endif

	}

	void Framebuffer::UnBind()
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglFramebuffer.UnBind();
#else

#endif

	}

	void Framebuffer::OnResize(const uint32_t width, const uint32_t height)
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		m_OpenglFramebuffer.OnResize(width, height);
#else

#endif

	}

	const FramebufferData& Framebuffer::GetData() const
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		return m_OpenglFramebuffer.GetData();
#else

#endif

	}

	uint32_t Framebuffer::GetColorAttachmentID() const
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		return m_OpenglFramebuffer.GetColorAttachmentID();
#else

#endif

	}

	uint32_t Framebuffer::GetRendererID() const
	{

#ifdef SMOLENGINE_OPENGL_IMPL

		return m_OpenglFramebuffer.GetRendererID();
#else

#endif

	}
}