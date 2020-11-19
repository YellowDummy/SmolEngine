#pragma once

#include "Core/Core.h"

#include <vulkan/vulkan.h>

namespace SmolEngine
{
	class VulkanInstance
	{
	public:

		VulkanInstance();

		~VulkanInstance();

		/// 
		/// Main
		/// 
		
		void Init();

	private:

		bool CreateAppInfo();

		bool CreateInstance(const VkApplicationInfo& info);

	public:

		/// 
		/// Getters
		/// 

		const VkInstance* GetInstance() const;

	private:

		VkInstance m_VKInstance = VK_NULL_HANDLE;

	private:

		friend class VulkanRendererAPI;
	};
}
