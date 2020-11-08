#pragma once
#include "Core/Core.h"
#include <cereal/cereal.hpp>

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

		virtual ~Framebuffer() {}

		/// Binding

		virtual void Bind() = 0;

		virtual void BindColorAttachment(uint32_t slot = 0) = 0;

		virtual void UnBind() = 0;

		/// Events

		virtual void OnResize(const uint32_t width, const uint32_t height) = 0;

		/// Getters

		virtual const FramebufferData& GetData() const = 0;

		virtual uint32_t GetColorAttachmentID() const = 0;

		virtual uint32_t GetRendererID() const = 0;

		/// Factory

		static Ref<Framebuffer> Create(const FramebufferData& data);
	};
}