#pragma once
#include "Core/Core.h"
#include "Core/Renderer/Vulkan/Vulkan.h"

namespace SmolEngine
{
	class VulkanShader;

	class VulkanDevice;

	class VulkanSwapchain;


	class VulkanPipeline
	{
	public:

		VulkanPipeline();

		~VulkanPipeline();

		/// 
		/// Main
		/// 
		
		bool Invalidate(const VulkanSwapchain* swapchain, const VulkanDevice* _device, const std::vector<VulkanShader*>& shaders);

		/// 
		/// Getters
		/// 

		const VkPipeline& GetVkPipeline();

	private:

		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;
		VkPipeline m_Pipeline = VK_NULL_HANDLE;
	};
}