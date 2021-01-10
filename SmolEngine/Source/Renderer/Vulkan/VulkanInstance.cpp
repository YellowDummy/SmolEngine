#include "stdafx.h"
#include "VulkanInstance.h"

#include <assert.h>
#include <vector>

#define VK_KHR_WIN32_SURFACE_EXTENSION_NAME "VK_KHR_win32_surface"

namespace SmolEngine
{
	VulkanInstance::VulkanInstance()
	{

	}

	VulkanInstance::~VulkanInstance()
	{
		vkDestroyInstance(m_VKInstance, nullptr);
	}

	void VulkanInstance::Init()
	{
		CreateAppInfo();
	}

	bool VulkanInstance::CreateAppInfo()
	{
		VkApplicationInfo appInfo = {};
		{
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

#ifdef  SMOLENGINE_EDITOR


			appInfo.pApplicationName = "SmolEngine Editor";
#else

			appInfo.pApplicationName = "SmolEngine Game";
#endif

			appInfo.pEngineName = "SmolEngine";
			appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_2;

		}

		return CreateInstance(appInfo);
	}

	bool VulkanInstance::CreateInstance(const VkApplicationInfo& appInfo)
	{
		std::vector<const char*> instanceLayers = { "VK_LAYER_KHRONOS_validation" };
		std::vector<const char*> instanceExt = { "VK_EXT_debug_report", VK_KHR_SURFACE_EXTENSION_NAME};

#ifdef _WIN32

		instanceExt.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
		
#endif // _WIN32


		VkInstanceCreateInfo instanceInfo = {};
		{
			instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instanceInfo.pApplicationInfo = &appInfo;
			instanceInfo.enabledExtensionCount = instanceExt.size();
			instanceInfo.ppEnabledExtensionNames = instanceExt.data();
			instanceInfo.enabledLayerCount = instanceLayers.size();
			instanceInfo.ppEnabledLayerNames = instanceLayers.data();
		}

		VkResult result = vkCreateInstance(&instanceInfo, nullptr, &m_VKInstance);

		assert(result == VK_SUCCESS);

		return result == VK_SUCCESS;
	}

	const VkInstance VulkanInstance::GetInstance() const
	{
		return m_VKInstance;
	}

}