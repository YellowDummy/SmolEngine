#pragma once
#ifndef FROSTIUM_OPENGL_IMPL
#include "Vulkan/Vulkan.h"

#include <vector>

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	class VulkanInstance;

	class VulkanDevice
	{
	public:

		VulkanDevice();
		~VulkanDevice();

		bool Init(const VulkanInstance* instance);

		// Getters
		uint32_t GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags memFlags) const;
		const VkPhysicalDeviceMemoryProperties* GetMemoryProperties() const;
		const VkPhysicalDeviceProperties* GetDeviceProperties() const;
		const VkPhysicalDeviceFeatures* GetDeviceFeatures() const;
		const VkSampleCountFlagBits GetMSAASamplesCount() const;
		const VkPhysicalDevice GetPhysicalDevice() const;
		const VkDevice GetLogicalDevice() const;
		uint32_t GetQueueFamilyIndex() const;
		const VkQueue GetQueue() const;
		bool GetRaytracingSupport() const;

	private:

		// Helpers
		bool SetupPhysicalDevice(const VulkanInstance* instance);
		bool SetupLogicalDevice();
		bool HasRequiredExtensions(const VkPhysicalDevice& device, const std::vector<const char*>& extensionsList);
		bool GetFamilyQueue(const VkPhysicalDevice& device, VkQueueFlags flags, uint32_t& outQueueIndex);
		void FindMaxUsableSampleCount();
		void SelectDevice(VkPhysicalDevice device);
		void GetFuncPtrs();

	private:

		VkPhysicalDevice                                 m_VkPhysicalDevice = VK_NULL_HANDLE;
		VkDevice                                         m_VkLogicalDevice = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties                       m_VkDeviceProperties = {};
		VkPhysicalDeviceFeatures                         m_VkDeviceFeatures = {};
		VkPhysicalDeviceMemoryProperties                 m_VkMemoryProperties = {};
		VkQueue                                          m_Queue = nullptr;

		// Function pointers for ray tracing related stuff
		PFN_vkGetBufferDeviceAddressKHR                  m_vkGetBufferDeviceAddressKHR;
		PFN_vkCreateAccelerationStructureKHR             m_vkCreateAccelerationStructureKHR;
		PFN_vkDestroyAccelerationStructureKHR            m_vkDestroyAccelerationStructureKHR;
		PFN_vkGetAccelerationStructureBuildSizesKHR      m_vkGetAccelerationStructureBuildSizesKHR;
		PFN_vkGetAccelerationStructureDeviceAddressKHR   m_vkGetAccelerationStructureDeviceAddressKHR;
		PFN_vkBuildAccelerationStructuresKHR             m_vkBuildAccelerationStructuresKHR;
		PFN_vkCmdBuildAccelerationStructuresKHR          m_vkCmdBuildAccelerationStructuresKHR;
		PFN_vkCmdTraceRaysKHR                            m_vkCmdTraceRaysKHR;
		PFN_vkGetRayTracingShaderGroupHandlesKHR         m_vkGetRayTracingShaderGroupHandlesKHR;
		PFN_vkCreateRayTracingPipelinesKHR               m_vkCreateRayTracingPipelinesKHR;

		bool                                             m_RayTracingEnabled = false;
		uint32_t                                         m_DeviceQueueFamilyIndex = 0;
		VkSampleCountFlagBits                            m_MSAASamplesCount = VK_SAMPLE_COUNT_1_BIT;
		std::vector<const char*>                         m_ExtensionsList;
										                 
	private:

		friend class VulkanRendererAPI;
		friend class VulkanCommandPool;
	};
}
#endif