#include "stdafx.h"
#include "VulkanDevice.h"
#include "Renderer/Vulkan/VulkanInstance.h"

#include <assert.h>

namespace SmolEngine
{
	VulkanDevice::VulkanDevice()
	{

	}

	VulkanDevice::~VulkanDevice()
	{
		if (m_VkLogicalDevice != VK_NULL_HANDLE)
		{
			//vkDestroyDevice(m_VkLogicalDevice, nullptr);
		}
	}

	bool VulkanDevice::Init(const VulkanInstance* instance)
	{
		if (SetupPhysicalDevice(instance))
		{
			NATIVE_INFO("Vulkan Info:\n\n                   Vulkan API Version: {}\n                   Selected Device: {}\n                   Driver Version: {}\n                   Max push_constant size: {}\n\n",
				m_VkDeviceProperties.apiVersion,
				std::string(m_VkDeviceProperties.deviceName),
				m_VkDeviceProperties.driverVersion,
				m_VkDeviceProperties.limits.maxPushConstantsSize);

			bool setup_result = SetupLogicalDevice();
			if (setup_result)
			{
				vkGetDeviceQueue(m_VkLogicalDevice, m_DeviceQueueFamilyIndex, 0, &m_Queue);
				FindMaxUsableSampleCount();
			}
			return setup_result;
		}

		return false;
	}

	bool VulkanDevice::SetupPhysicalDevice(const VulkanInstance* _instance)
	{
		m_ExtensionsList = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		const VkInstance& instance = _instance->GetInstance();

		uint32_t devicesCount = 0;
		vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr);

		//

		std::unique_ptr<VkPhysicalDevice[]> devices = std::make_unique<VkPhysicalDevice[]>(devicesCount);
		vkEnumeratePhysicalDevices(instance, &devicesCount, devices.get());

		for (uint32_t i = 0; i < devicesCount; ++i)
		{
			const VkPhysicalDevice& current_device = devices[i];

			if (HasRequiredExtensions(current_device, m_ExtensionsList))
			{
				uint32_t familyIndex = 0;

				if (GetFamilyQueue(current_device, VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT, familyIndex))
				{
					VkPhysicalDeviceProperties deviceProperties = {};
					{
						vkGetPhysicalDeviceProperties(current_device, &deviceProperties);
					}

					VkPhysicalDeviceFeatures deviceFeatures = {};
					{
						vkGetPhysicalDeviceFeatures(current_device, &deviceFeatures);
					}

					VkPhysicalDeviceMemoryProperties memoryProperties = {};
					{
						vkGetPhysicalDeviceMemoryProperties(current_device, &memoryProperties);
					}

					if (m_VkPhysicalDevice == VK_NULL_HANDLE || deviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
					{
						m_VkDeviceProperties = deviceProperties;
						m_VkDeviceFeatures = deviceFeatures;
						m_VkMemoryProperties = memoryProperties;

						m_VkDeviceFeatures.shaderSampledImageArrayDynamicIndexing = VK_TRUE;

						m_VkPhysicalDevice = current_device;
						m_DeviceQueueFamilyIndex = familyIndex;
					}
				}
			}
		}

		assert(m_VkPhysicalDevice != VK_NULL_HANDLE);
		return m_VkPhysicalDevice != VK_NULL_HANDLE;
	}


	bool VulkanDevice::SetupLogicalDevice()
	{
		static const float priority = 1.0f;

		VkDeviceQueueCreateInfo queueCreateInfo = {};
		{
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = m_DeviceQueueFamilyIndex;
			queueCreateInfo.queueCount = 1; // temp
			queueCreateInfo.pQueuePriorities = &priority; // temp

		}

		VkDeviceCreateInfo deviceInfo = {};
		{
			deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceInfo.pQueueCreateInfos = &queueCreateInfo;
			deviceInfo.queueCreateInfoCount = 1;
			deviceInfo.pEnabledFeatures = &m_VkDeviceFeatures;
			deviceInfo.enabledExtensionCount = m_ExtensionsList.size();
			deviceInfo.ppEnabledExtensionNames = m_ExtensionsList.data();
		}

VkResult result = vkCreateDevice(m_VkPhysicalDevice, &deviceInfo, nullptr, &m_VkLogicalDevice);
assert(result == VK_SUCCESS);

return result == VK_SUCCESS;
	}

	bool VulkanDevice::HasRequiredExtensions(const VkPhysicalDevice& device, const std::vector<const char*>& extensionsList)
	{
		uint32_t extCount = 0;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);

		//

		std::unique_ptr<VkExtensionProperties[]> extensions = std::make_unique<VkExtensionProperties[]>(extCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, extensions.get());

		for (const auto& name : extensionsList)
		{
			bool ext_found = false;

			for (uint32_t i = 0; i < extCount; ++i)
			{
				// Note:
				// The return value from strcmp is 0 if the two strings are equal

				if (strcmp(extensions[i].extensionName, name) == 0)
				{
					ext_found = true;
					break;
				}
			}

			if (ext_found == false)
			{
				return false;
			}
		}

		return true;
	}

	bool VulkanDevice::GetFamilyQueue(const VkPhysicalDevice& device, VkQueueFlags flags, uint32_t& outQueueIndex)
	{

		uint32_t queueCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);

		//

		std::unique_ptr<VkQueueFamilyProperties[]> families = std::make_unique<VkQueueFamilyProperties[]>(queueCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, families.get());

		for (uint32_t i = 0; i < queueCount; ++i)
		{
			if (families[i].queueCount > 0)
			{
				if ((families[i].queueFlags & flags) == flags)
				{
					outQueueIndex = i;

					return true;
				}
			}
		}

		return false;
	}

	void VulkanDevice::FindMaxUsableSampleCount()
	{
		VkSampleCountFlagBits sampleCount;
		VkSampleCountFlags counts = m_VkDeviceProperties.limits.framebufferColorSampleCounts & m_VkDeviceProperties.limits.framebufferDepthSampleCounts;
		if (counts & VK_SAMPLE_COUNT_64_BIT) { sampleCount = VK_SAMPLE_COUNT_64_BIT; }
		else if (counts & VK_SAMPLE_COUNT_32_BIT) { sampleCount = VK_SAMPLE_COUNT_32_BIT; }
		else if (counts & VK_SAMPLE_COUNT_16_BIT) { sampleCount = VK_SAMPLE_COUNT_16_BIT; }
		else if (counts & VK_SAMPLE_COUNT_8_BIT) { sampleCount = VK_SAMPLE_COUNT_8_BIT; }
		else if (counts & VK_SAMPLE_COUNT_4_BIT) { sampleCount = VK_SAMPLE_COUNT_4_BIT; }
		else if (counts & VK_SAMPLE_COUNT_2_BIT) { sampleCount = VK_SAMPLE_COUNT_2_BIT; }
		else { sampleCount = VK_SAMPLE_COUNT_1_BIT; }

		m_VkDeviceFeatures.sampleRateShading = VK_TRUE;
		m_MSAASamplesCount = sampleCount;
	}

	const VkPhysicalDeviceMemoryProperties* const VulkanDevice::GetMemoryProperties() const
	{
		return &m_VkMemoryProperties;
	}

	const VkPhysicalDeviceProperties* const VulkanDevice::GetDeviceProperties() const
	{
		return &m_VkDeviceProperties;
	}

	const VkPhysicalDeviceFeatures* const VulkanDevice::GetDeviceFeatures() const
	{
		return &m_VkDeviceFeatures;
	}

	const VkSampleCountFlagBits VulkanDevice::GetMSAASamplesCount() const
	{
		return m_MSAASamplesCount;
	}

	const VkPhysicalDevice const VulkanDevice::GetPhysicalDevice() const
	{
		return m_VkPhysicalDevice;
	}

	const VkDevice const VulkanDevice::GetLogicalDevice() const
	{
		return m_VkLogicalDevice;
	}

	uint32_t VulkanDevice::GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags memFlags) const
	{
		for (uint32_t i = 0; i < m_VkMemoryProperties.memoryTypeCount; ++i)
		{
			if ((typeBits & 1) == 1)
			{
				if ((m_VkMemoryProperties.memoryTypes[i].propertyFlags & memFlags) == memFlags)
				{
					return i;
				}
			}

			typeBits >>= 1;
		}

		NATIVE_ERROR("Could not find a suitable memory type!");
		abort();
	}

	uint32_t VulkanDevice::GetQueueFamilyIndex() const
	{
		return m_DeviceQueueFamilyIndex;
	}

	const VkQueue VulkanDevice::GetQueue() const
	{
		return m_Queue;
	}
}