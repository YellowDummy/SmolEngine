#pragma once
#include "Core/Renderer/Framebuffer.h"

namespace SmolEngine
{
	class OpenglFramebuffer: public Framebuffer
	{
	public:

		OpenglFramebuffer(const FramebufferData& data);

		~OpenglFramebuffer();

		///

		void Recreate();

		/// Binding
		
		void Bind() override;

		void UnBind() override;

		void BindColorAttachment(uint32_t slot = 0) override;

		/// Events

		void OnResize(const uint32_t width, const uint32_t height) override;

		/// Getters

		const FramebufferData& GetData() const override { return m_Data; }

		uint32_t GetColorAttachmentID() const override { return m_ColorAttachment; }

		uint32_t GetRendererID() const override { return m_RendererID; }

	private:

		FramebufferData m_Data;

		uint32_t m_RendererID = 0, m_ColorAttachment = 0, m_DepthAttachment = 0;

		const uint32_t m_MaxSize = 8192;
	};
}
