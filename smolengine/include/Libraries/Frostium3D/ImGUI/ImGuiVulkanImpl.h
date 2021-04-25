#pragma once
#ifndef FROSTIUM_OPENGL_IMPL
#include "Vulkan/Vulkan.h"
#include <imgui/examples/imgui_impl_vulkan.h>

struct GLFWwindow;

namespace Frostium
{
	class VulkanCommandBuffer;
	class ImGuiVulkanImpl
	{
	public:

		void Init();
		void Reset();
		void InitResources();


		VkDevice         g_Device;
		VkPipelineCache  g_PipelineCache = VK_NULL_HANDLE;
		VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;
	};
}

#endif