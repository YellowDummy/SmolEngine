#pragma once
#include "Core/Core.h"

#include <vector>

namespace SmolEngine
{
	class VertexBuffer;
	class IndexBuffer;

	class OpenglVertexArray
	{
	public:

		OpenglVertexArray();

		~OpenglVertexArray();

		/// Init

		void Init();

		/// Binding

		void Bind() const;

		void UnBind() const;

		/// Setters

		void SetVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);

		void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer);

		/// Getters

		Ref<IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer; }

	private:

		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer = nullptr;
		uint32_t m_RendererID = UINT32_MAX;
		bool m_IsInitialized = false;
	};
}
