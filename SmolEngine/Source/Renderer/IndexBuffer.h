#pragma once
#include "Core/Core.h"
#include "Renderer/BufferLayout.h"

#ifdef SMOLENGINE_OPENGL_IMPL
#include "Renderer/OpenGL/OpenglBuffer.h"
#else
#include "Renderer/Vulkan/VulkanIndexBuffer.h"
#endif
namespace SmolEngine
{
	class IndexBuffer
	{
	public:

		IndexBuffer() = default;

		~IndexBuffer() = default;


		void Bind() const;

		void UnBind() const;


		void UploadData(uint32_t* indices, uint32_t count);

#ifndef SMOLENGINE_OPENGL_IMPL

		void CmdUpdateData(VkCommandBuffer cmdBuffer, const void* data, size_t size, uint32_t offset = 0)
		{
			m_VulkanIndexBuffer.CmdUpdateData(cmdBuffer, data, size, offset);
		}

#endif

		void Destory();

		///  Getters

		uint32_t GetCount() const;

#ifndef SMOLENGINE_OPENGL_IMPL

		VulkanIndexBuffer& GetVulkanIndexBuffer() { return m_VulkanIndexBuffer; };
#endif

		/// Factory

		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);

	private:

#ifdef SMOLENGINE_OPENGL_IMPL

		OpenglIndexBuffer m_OpenglIndexBuffer = {};
#else
		VulkanIndexBuffer m_VulkanIndexBuffer = {};
#endif
	};
}