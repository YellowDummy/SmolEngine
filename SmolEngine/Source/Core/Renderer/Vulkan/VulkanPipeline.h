#pragma once
#include "Core/Core.h"
#include "Core/Renderer/Vulkan/Vulkan.h"

namespace SmolEngine
{
	struct VulkanPipelineSpecification;

	class VulkanPipeline
	{
	public:

		VulkanPipeline();

		~VulkanPipeline();

		/// 
		/// Main
		/// 
		
		bool Invalidate(const VulkanPipelineSpecification* pipelineSpec);

		/// 
		/// Getters
		/// 

		const VkPipeline& GetVkPipeline() const;

		const VkPipelineLayout& GetVkPipelineLayot() const;

	private:

		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;
		VkPipeline m_Pipeline = VK_NULL_HANDLE;
	};
}