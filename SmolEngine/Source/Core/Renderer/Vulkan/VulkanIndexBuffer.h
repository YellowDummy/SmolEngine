#pragma once
#include "Core/Renderer/Vulkan/Vulkan.h"
#include "Core/Renderer/Vulkan/VulkanBuffer.h"

namespace SmolEngine
{
	class VulkanIndexBuffer
	{
	public:

		VulkanIndexBuffer();

		~VulkanIndexBuffer();

		/// 
		/// Main
		/// 

		void Create(const void* data, uint64_t size);

		void SetData(const void* data, uint64_t size, uint32_t offset = 0);

		/// 
		/// Getters
		/// 

		uint32_t GetSize() const;

		const VkBuffer& GetBuffer() const;

	private:

		VulkanBuffer m_IndexBuffer = {};

	};
}