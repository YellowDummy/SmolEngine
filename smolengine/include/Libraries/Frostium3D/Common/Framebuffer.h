#pragma once
#include "Common/Core.h"

#include "Common/FramebufferSpecification.h"

#ifdef FROSTIUM_OPENGL_IMPL
#include "OpenGL/OpenglFramebuffer.h"
#else
#include "Vulkan/VulkanFramebuffer.h"
#endif

namespace Frostium
{
	class Framebuffer
	{
	public:

		Framebuffer() = default;
		~Framebuffer() = default;

		// Binding
		void Bind();
		void BindColorAttachment(uint32_t slot = 0);
		void UnBind();

		// Events
		void OnResize(const uint32_t width, const uint32_t height);

		//  Getters
		const FramebufferSpecification& GetSpecification() const;
		void* GetImGuiTextureID(uint32_t index = 0);
		uint32_t GetColorAttachmentID() const;
		uint32_t GetRendererID() const;
#ifndef FROSTIUM_OPENGL_IMPL
		VulkanFramebuffer& GetVulkanFramebuffer() { return m_VulkanFrameBuffer; }
#endif
		// Factory
		static Ref<Framebuffer> Create(const FramebufferSpecification& data);
		static void Create(const FramebufferSpecification& data, Framebuffer* out_fb);

	private:
#ifdef FROSTIUM_OPENGL_IMPL
		OpenglFramebuffer m_OpenglFramebuffer;
#else
		VulkanFramebuffer m_VulkanFrameBuffer;
#endif
	};
}