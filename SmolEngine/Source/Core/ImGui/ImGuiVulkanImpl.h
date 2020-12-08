#pragma once

#include "Core/Renderer/Vulkan/Vulkan.h"
#include <../Libraries/imgui/examples/imgui_impl_vulkan.h>
#include "Core/Renderer/Vulkan/VulkanSwapchain.h"

struct GLFWwindow;

namespace SmolEngine
{
	class VulkanCommandBuffer;

	class ImGuiVulkanImpl
	{
	public:

		ImGuiVulkanImpl();

		~ImGuiVulkanImpl();

		/// 
		/// Main
		/// 
		
		void Init();

		void Reset();

		void InitResources();

	private:

		VkDevice g_Device;
		VkPipelineCache  g_PipelineCache = VK_NULL_HANDLE;
		VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;

		friend class ImGuiLayer;
	};
}