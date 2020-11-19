#pragma once
#include "Core/Core.h"

#include "Core/Renderer/Vulkan/VulkanInstance.h"
#include "Core/Renderer/Vulkan/VulkanDevice.h"
#include "Core/Renderer/Vulkan/VulkanCommandPool.h"

namespace SmolEngine
{
	class VulkanRendererAPI
	{
	public:

		/// 
		/// Main
		/// 

		void Init();

	private:

		VulkanCommandPool m_CommandPool = {};

		VulkanInstance m_Instance = {};

		VulkanDevice m_Device = {};
	};
}