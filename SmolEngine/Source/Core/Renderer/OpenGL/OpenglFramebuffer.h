#pragma once
#include "Core/Renderer/Framebuffer.h"

namespace SmolEngine
{
	class OpenglFramebuffer: public Framebuffer
	{
	public:
		OpenglFramebuffer(const FramebufferData& data);
		~OpenglFramebuffer();

		void Recreate();
		void Bind() override;
		void UnBind() override;
		void OnResize(const uint32_t width, const uint32_t height) override;

		const FramebufferData& GetData() const override { return m_Data; }
		uint32_t GetColorAttachmentID() const override { return m_ColorAttachment; }

	private:
		FramebufferData m_Data;
		uint32_t m_RendererID = 0, m_ColorAttachment = 0, m_DepthAttachment = 0;
		const uint32_t m_MaxSize = 8192;
	};
}
