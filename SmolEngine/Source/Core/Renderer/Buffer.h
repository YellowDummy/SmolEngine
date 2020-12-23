#pragma once

#include "Core/Core.h"
#include <vector>
#include <string>
#include <memory>

#include "Core/Renderer/BufferLayout.h"

#ifdef SMOLENGINE_OPENGL_IMPL

#include "Core/Renderer/OpenGL/OpenglBuffer.h"
#include "Core/Renderer/OpenGL/OpenglVertexArray.h"

#else

#include "Core/Renderer/Vulkan/VulkanVertexBuffer.h"
#include "Core/Renderer/Vulkan/VulkanIndexBuffer.h"

#endif

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

		void Destory();

		///
		/// Data
		/// 

		void UploadData(const void* data, const uint32_t size, const uint32_t offset = 0);

		/// 
		/// Getters
		/// 

		const BufferLayout& GetLayout() const;

#ifndef SMOLENGINE_OPENGL_IMPL

		VulkanVertexBuffer& GetVulkanVertexBuffer() { return m_VulkanVextexBuffer; }
#endif

		/// 
		/// Setters
		/// 

		void SetLayout(const BufferLayout& layout);

		/// 
		/// Factory
		/// 

		static Ref<VertexBuffer> Create(uint32_t size);

		static Ref<VertexBuffer> Create(void* vertices, uint32_t size);

	private:

#ifdef SMOLENGINE_OPENGL_IMPL
		OpenglVertexBuffer m_OpenglVertexBuffer = {};
#else
		VulkanVertexBuffer m_VulkanVextexBuffer = {};
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

		void UploadData(uint32_t* indices, uint32_t count);

		void Destory();

		///
		///  Getters
		/// 

		uint32_t GetCount() const;

#ifndef SMOLENGINE_OPENGL_IMPL

		VulkanIndexBuffer& GetVulkanIndexBuffer() { return m_VulkanIndexBuffer; };
#endif

		/// 
		/// Factory
		/// 

		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);

	private:

#ifdef SMOLENGINE_OPENGL_IMPL

		OpenglIndexBuffer m_OpenglIndexBuffer = {};
#else
		VulkanIndexBuffer m_VulkanIndexBuffer = {};
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

#ifdef  SMOLENGINE_OPENGL_IMPL

		OpenglVertexArray m_OpenglVertexArray = {};

#endif

	};
}