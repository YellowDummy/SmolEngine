#pragma once

#include "Core/Core.h"
#include <vector>
#include <string>
#include <memory>

#include "Renderer/BufferLayout.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"

#ifdef SMOLENGINE_OPENGL_IMPL
#include "Renderer/OpenGL/OpenglVertexArray.h"
#endif

namespace SmolEngine
{
	class VertexArray
	{
	public:

		VertexArray() = default;

		~VertexArray() = default;

		/// Binding
		
		void Bind() const;

		void UnBind() const;

		/// Setters

		void SetVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);

		void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer);

		/// Getters

		Ref<IndexBuffer> GetIndexBuffer() const;

		/// Factory

		static Ref<VertexArray> Create();

	private:

#ifdef  SMOLENGINE_OPENGL_IMPL
		OpenglVertexArray m_OpenglVertexArray = {};
#endif

	};
}