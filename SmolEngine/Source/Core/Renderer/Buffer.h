#pragma once

#include "Core/Core.h"
#include <vector>
#include <string>
#include <memory>

#include "Core/Renderer/OpenGL/OpenglBuffer.h"
#include "Core/Renderer/OpenGL/OpenglVertexArray.h"


namespace SmolEngine
{
	/// 
	/// VERTEX BUFFER
	///

	class VertexBuffer
	{
	public:

		VertexBuffer() = default;

		~VertexBuffer() = default;

		///
		///  Binding
		/// 

		void Bind() const;

		void UnBind() const;

		///
		/// Data
		/// 

		void UploadData(const void* data, const uint32_t size, const uint32_t offset = 0) const;

		/// 
		/// Getters
		/// 

		const BufferLayout& GetLayout() const;

		/// 
		/// Setters
		/// 

		void SetLayout(const BufferLayout& layout);

		/// 
		/// Factory
		/// 

		static Ref<VertexBuffer> Create(uint32_t size);

		static Ref<VertexBuffer> Create(float* vertices, uint32_t size);

	private:

#ifdef SMOLENGINE_OPENGL_IMPL

		OpenglVertexBuffer m_OpenglVertexBuffer = {};

#else
		// Vulkan

#endif

	};

	/// 
	/// INDEX BUFFER
	/// 
	
	class IndexBuffer
	{
	public:

		IndexBuffer() = default;

		~IndexBuffer() = default;

		/// 
		/// Binding
		/// 

		void Bind() const;

		void UnBind() const;

		///
		///  Getters
		/// 

		uint32_t GetCount() const;

		/// 
		/// Factory
		/// 

		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);

	private:

#ifdef SMOLENGINE_OPENGL_IMPL

		OpenglIndexBuffer m_OpenglIndexBuffer = {};
#else

		// Vulkan

#endif
	};

	class VertexArray
	{
	public:

		VertexArray() = default;

		~VertexArray() = default;

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

		Ref<IndexBuffer> GetIndexBuffer() const;

		/// 
		/// Factory
		/// 

		static Ref<VertexArray> Create();

	private:

		OpenglVertexArray m_OpenglVertexArray = {};

	};
}