#include "stdafx.h"
#include "Framebuffer.h"
#include "Core/Renderer/OpenGL/OpenglFramebuffer.h"
#include "Core/Renderer/Renderer.h"
#include "Core/SLog.h"

namespace SmolEngine
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& data)
	{
		Ref<Framebuffer> framebuffer = std::make_shared<Framebuffer>();
#ifdef SMOLENGINE_OPENGL_IMPL
		framebuffer->m_OpenglFramebuffer.Init(data);
#else
		framebuffer->m_VulkanFrameBuffer.Init(data);
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
		m_VulkanFrameBuffer.OnResize(width, height);
#endif
	}

	const FramebufferSpecification& Framebuffer::GetSpecification() const
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		return m_OpenglFramebuffer.GetData();
#else
		return m_VulkanFrameBuffer.GetSpecification();
#endif
	}

	uint32_t Framebuffer::GetColorAttachmentID() const
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		return m_OpenglFramebuffer.GetColorAttachmentID();
#else
		return 0;
#endif
	}

	void* Framebuffer::GetImGuiTextureID() const
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		return (void*)(intptr_t)m_OpenglFramebuffer.GetColorAttachmentID();
#else
		return m_VulkanFrameBuffer.GetImGuiTextureID();
#endif
	}

	uint32_t Framebuffer::GetRendererID() const
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		return m_OpenglFramebuffer.GetRendererID();
#else
		return 0;
#endif
	}
}