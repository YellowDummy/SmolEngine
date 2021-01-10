#pragma once
#include "Core/Core.h"
#include <cereal/cereal.hpp>

#include "Renderer/FramebufferSpecification.h"
#include "Renderer/OpenGL/OpenglFramebuffer.h"
#include "Renderer/Vulkan/VulkanFramebuffer.h"

namespace SmolEngine
{
	class Framebuffer
	{
	public:

		Framebuffer() = default;

		~Framebuffer() = default;

		/// 
		/// Binding
		/// 

		void Bind();

		void BindColorAttachment(uint32_t slot = 0);

		void UnBind();

		/// 
		/// Events
		/// 

		void OnResize(const uint32_t width, const uint32_t height);

		///
		///  Getters
		/// 

		const FramebufferSpecification& GetSpecification() const;

		uint32_t GetColorAttachmentID() const;

		void* GetImGuiTextureID() const;

		uint32_t GetRendererID() const;

#ifndef SMOLENGINE_OPENGL_IMPL

		VulkanFramebuffer& GetVulkanFramebuffer() { return m_VulkanFrameBuffer; }
#endif

		/// 
		/// Factory
		/// 

		static Ref<Framebuffer> Create(const FramebufferSpecification& data);

	private:

#ifdef SMOLENGINE_OPENGL_IMPL

		OpenglFramebuffer m_OpenglFramebuffer;
#else
		VulkanFramebuffer m_VulkanFrameBuffer;
#endif
	};
}