#pragma once
#ifndef FROSTIUM_OPENGL_IMPL
#include "Vulkan/Vulkan.h"

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	class VulkanInstance
	{
	public:

		VulkanInstance();
		~VulkanInstance();

		void Init();

		// Getters
		const VkInstance GetInstance() const;

	private:

		bool CreateAppInfo();
		bool CreateInstance(const VkApplicationInfo& info);

	private:

		VkInstance m_VKInstance = VK_NULL_HANDLE;

		friend class VulkanRendererAPI;
	};
}

#endif