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

		void Create(const uint32_t* data, uint64_t count);

		void Create(uint64_t size);

		void SetData(const uint32_t* data, uint64_t count);


		void* MapMemory();

		void UnMapMemory();

		void Destroy();

		/// 
		/// Getters
		/// 

		uint32_t GetSize() const;

		uint32_t GetCount() const;

		const VkBuffer& GetBuffer() const;

	private:

		uint32_t m_ElementsCount = 0;

		VulkanBuffer m_IndexBuffer = {};

	};
}