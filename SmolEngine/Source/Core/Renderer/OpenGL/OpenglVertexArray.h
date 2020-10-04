#pragma once

#include "Core/Core.h"
#include "Core/Renderer/Buffer.h"

namespace SmolEngine
{
	class OpenglVertexArray : public VertexArray
	{
	public:

		OpenglVertexArray();

		~OpenglVertexArray();

		void Bind() const override;

		void UnBind() const override;

		void SetVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;

		void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)  override;

		Ref<IndexBuffer> GetIndexBuffer() const override { return m_IndexBuffer; }

	private:

		uint32_t m_RendererID;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}
