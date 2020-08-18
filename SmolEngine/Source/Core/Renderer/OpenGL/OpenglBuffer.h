#pragma once
#include "Core/Renderer/Buffer.h"

namespace SmolEngine
{
	class BufferLayout;

	class OpenglVertexBuffer: public VertexBuffer
	{
	public:
		OpenglVertexBuffer(uint32_t size);
		OpenglVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenglVertexBuffer();
		void Bind() const override;
		void UnBind() const override;
		void UploadData(const void* data, const uint32_t size, const uint32_t offset = 0) const override;

		const BufferLayout& GetLayout() const override { return m_Layout; }
		void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
	};

	class OpenglIndexBuffer : public IndexBuffer
	{
	public:
		OpenglIndexBuffer(uint32_t* indices, uint32_t size);
		virtual ~OpenglIndexBuffer();
		void Bind() const override;
		void UnBind() const override;
		uint32_t GetCount() const override { return m_Count; };
	private:
		uint32_t m_Count;
		uint32_t m_RendererID;
	};
}
