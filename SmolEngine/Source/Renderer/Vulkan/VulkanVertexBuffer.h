#pragma once
#include "Renderer/Vulkan/Vulkan.h"
#include "Renderer/Vulkan/VulkanBuffer.h"

namespace SmolEngine
{
	class VulkanVertexBuffer
	{
	public:

		VulkanVertexBuffer();

		~VulkanVertexBuffer();

		/// Main

		void Create(const void* data, uint64_t size);

		void Create(uint64_t size);

		void SetData(const void* data, uint64_t size, uint32_t offset = 0);

		void CmdUpdateData(VkCommandBuffer cmdBuffer, const void* data, size_t size, uint32_t offset = 0);

		void* MapMemory();

		void UnMapMemory();

		void Destroy();

		/// Getters

		uint32_t GetSize() const;

		const VkBuffer& GetBuffer() const;

	private:

		VulkanBuffer m_VertexBuffer = {};

	};
}