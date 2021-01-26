#pragma once
#include "Core/Core.h"
#include "Renderer/Vulkan/Vulkan.h"

namespace SmolEngine
{
	class VulkanInstance;

	class VulkanDevice
	{
	public:

		VulkanDevice();

		~VulkanDevice();

		/// Main
		
		bool Init(const VulkanInstance* instance);

		/// Getters

		uint32_t GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags memFlags) const;

		const VkPhysicalDeviceMemoryProperties* GetMemoryProperties() const;

		const VkPhysicalDeviceProperties* GetDeviceProperties() const;

		const VkPhysicalDeviceFeatures* GetDeviceFeatures() const;

		const VkSampleCountFlagBits GetMSAASamplesCount() const;

		const VkPhysicalDevice GetPhysicalDevice() const;

		const VkDevice GetLogicalDevice() const;

		uint32_t GetQueueFamilyIndex() const;

		const VkQueue GetQueue() const;
		
	private:

		bool SetupPhysicalDevice(const VulkanInstance* instance);

		bool SetupLogicalDevice();

		/// Helpers

		bool HasRequiredExtensions(const VkPhysicalDevice& device, const std::vector<const char*>& extensionsList);

		bool GetFamilyQueue(const VkPhysicalDevice& device, VkQueueFlags flags, uint32_t& outQueueIndex);

		void FindMaxUsableSampleCount();

	private:

		VkPhysicalDevice                   m_VkPhysicalDevice = VK_NULL_HANDLE;
		VkDevice                           m_VkLogicalDevice = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties         m_VkDeviceProperties = {};
		VkPhysicalDeviceFeatures           m_VkDeviceFeatures = {};
		VkPhysicalDeviceMemoryProperties   m_VkMemoryProperties = {};
		VkQueue                            m_Queue = nullptr;

		uint32_t                           m_DeviceQueueFamilyIndex = 0;
		VkSampleCountFlagBits              m_MSAASamplesCount = VK_SAMPLE_COUNT_2_BIT;
		std::vector<const char*>           m_ExtensionsList;

		friend class VulkanRendererAPI;
		friend class VulkanCommandPool;
	};
}