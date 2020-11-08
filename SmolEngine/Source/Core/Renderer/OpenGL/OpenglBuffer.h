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

		/// Binding

		void Bind() const override;

		void UnBind() const override;

		///

		void UploadData(const void* data, const uint32_t size, const uint32_t offset = 0) const override;

		/// Getters

		const BufferLayout& GetLayout() const override { return m_Layout; }

		/// Setters

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

		/// Binding

		void Bind() const override;

		void UnBind() const override;

		/// Getters

		uint32_t GetCount() const override { return m_Count; };

	private:

		uint32_t m_Count;

		uint32_t m_RendererID;
	};
}
