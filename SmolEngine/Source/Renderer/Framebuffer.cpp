#include "stdafx.h"
#include "Framebuffer.h"
#include "Renderer/OpenGL/OpenglFramebuffer.h"
#include "Renderer/Renderer.h"
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
		m_OpenglFramebuffer.OnResize(height, width);
#else
		m_VulkanFrameBuffer.OnResize(height, width);
#endif
	}

	const FramebufferSpecification& Framebuffer::GetSpecification() const
	{
#ifdef SMOLENGINE_OPENGL_IMPL
		return m_OpenglFramebuffer.GetSpecification();
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

	void* Framebuffer::GetImGuiTextureID(uint32_t index)
	{
#ifdef SMOLENGINE_OPENGL_IMPL

		return reinterpret_cast<void*>(m_OpenglFramebuffer.GetColorAttachmentID());
#else
		return m_VulkanFrameBuffer.GetAttachment(index)->ImGuiID;
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