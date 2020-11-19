#pragma once
#include "Core/Core.h"
#include <cereal/cereal.hpp>

#include "Core/Renderer/OpenGL/OpenglFramebuffer.h"

namespace SmolEngine
{
	struct FramebufferData
	{
		uint32_t Width;

		uint32_t Height;

		uint32_t Samples = 1;

		bool SwapChainEnabled = false; // Vulkan
	};

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

		const FramebufferData& GetData() const;

		uint32_t GetColorAttachmentID() const;

		uint32_t GetRendererID() const;

		/// 
		/// Factory
		/// 

		static Ref<Framebuffer> Create(const FramebufferData& data);

	private:

#ifdef SMOLENGINE_OPENGL_IMPL

		OpenglFramebuffer m_OpenglFramebuffer;
#else

#endif
	};
}