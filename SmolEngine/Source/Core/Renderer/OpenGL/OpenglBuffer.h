#pragma once

#include "Core/Renderer/BufferLayout.h"

namespace SmolEngine
{
	class OpenglVertexBuffer
	{
	public:

		OpenglVertexBuffer();

		~OpenglVertexBuffer();

		///
		/// Init
		/// 

		void Init(uint32_t size);

		void Init(void* vertices, uint32_t size);

		void Destroy();

		/// 
		/// Binding
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

		const BufferLayout& GetLayout() const { return m_Layout; }

		/// 
		/// Setters
		/// 

		void SetLayout(const BufferLayout& layout) { m_Layout = layout; }


	private:

		uint32_t m_RendererID;

		BufferLayout m_Layout;
	};

	class OpenglIndexBuffer
	{
	public:

		OpenglIndexBuffer();

		~OpenglIndexBuffer();

		///
		/// Init
		///
		
		void Init(uint32_t* indices, uint32_t count);

		void Destroy();

		/// 
		/// Binding
		/// 

		void Bind() const;

		void UnBind() const;

		/// 
		/// Getters
		/// 

		uint32_t GetCount() const { return m_Count; };

	private:

		uint32_t m_Count;

		uint32_t m_RendererID;
	};
}
