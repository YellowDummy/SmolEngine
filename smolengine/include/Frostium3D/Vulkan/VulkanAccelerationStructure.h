#pragma once
#ifndef FROSTIUM_OPENGL_IMPL
#include "Vulkan/Vulkan.h"

namespace Frostium
{
	class VulkanAccelerationStructure
	{
	public:

		VulkanAccelerationStructure() = default;
		~VulkanAccelerationStructure();

		void Create(VkAccelerationStructureTypeKHR type, VkAccelerationStructureBuildSizesInfoKHR buildSizeInfo);
		void Delete();

		// Getters
		uint64_t GetDeviceAddress() const;

	private:

		VkDevice                     m_Device = nullptr;
		VkAccelerationStructureKHR   m_Structure = nullptr;
		VkDeviceMemory               m_Memory = nullptr;
		VkBuffer                     m_Buffer = nullptr;
		uint64_t                     m_DeviceAddress = 0;
	};
}
#endif