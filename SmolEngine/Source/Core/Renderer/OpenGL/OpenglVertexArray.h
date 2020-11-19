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

		/// 
		/// Binding
		/// 

		void Bind() const;

		void UnBind() const;


		/// 
		/// Setters
		/// 

		void SetVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);

		void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer);

		/// 
		/// Getters
		/// 

		Ref<IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer; }

	private:

		std::vector<Ref<VertexBuffer>> m_VertexBuffers;

		Ref<IndexBuffer> m_IndexBuffer;

		uint32_t m_RendererID;
	};
}
