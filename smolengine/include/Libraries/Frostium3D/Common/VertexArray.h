#pragma once

#include "Common/Core.h"
#include "Common/BufferLayout.h"
#include "Common/VertexBuffer.h"
#include "Common/IndexBuffer.h"

#include <vector>
#include <string>
#include <memory>

#ifdef FROSTIUM_OPENGL_IMPL
#include "OpenGL/OpenglBuffer.h"
#include "OpenGL/OpenglVertexArray.h"
#endif

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	class VertexArray
	{
	public:

		VertexArray() = default;
		~VertexArray() = default;

		// Binding
		void Bind() const;
		void UnBind() const;

		// Setters
		void SetVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);
		void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer);

		// Getters
		Ref<IndexBuffer> GetIndexBuffer() const;

		// Factory
		static Ref<VertexArray> Create();

	private:

#ifdef  FROSTIUM_OPENGL_IMPL
		OpenglVertexArray m_OpenglVertexArray = {};
#endif

	};
}