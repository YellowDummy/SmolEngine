#pragma once

namespace SmolEngine
{
	struct FramebufferData;

	class OpenglFramebuffer
	{
	public:

		OpenglFramebuffer();

		~OpenglFramebuffer();

		/// 
		/// Init
		/// 

		void Init(const FramebufferData& data);

		void Recreate();

		/// 
		/// Binding
		/// 
		
		void Bind();

		void UnBind();

		void BindColorAttachment(uint32_t slot = 0);

		///
		///  Events
		/// 

		void OnResize(const uint32_t width, const uint32_t height);

		/// 
		/// Getters
		/// 

		const FramebufferData& GetData() const;

		uint32_t GetColorAttachmentID() const;

		uint32_t GetRendererID() const;

	private:

		FramebufferData* m_Data = nullptr;

		uint32_t m_RendererID = 0, m_ColorAttachment = 0, m_DepthAttachment = 0;

		const uint32_t m_MaxSize = 8192;
	};
}
