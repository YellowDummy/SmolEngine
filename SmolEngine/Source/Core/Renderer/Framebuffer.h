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
		bool SwapChainEnabled = false; //Vulkan support
	};

	class Framebuffer
	{
	public:

		virtual ~Framebuffer() {}

		virtual void Bind() = 0;

		virtual void UnBind() = 0;

		virtual void OnResize(const uint32_t width, const uint32_t height) = 0;

		virtual const FramebufferData& GetData() const = 0;

		virtual uint32_t GetColorAttachmentID() const = 0;

		static Ref<Framebuffer> Create(const FramebufferData& data);
	};
}