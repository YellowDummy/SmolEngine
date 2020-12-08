#include "stdafx.h"
#include "ImGuiVulkanImpl.h"

#include <../Libraries/imgui/imgui.h>
#include "Core/Renderer/Vulkan/VulkanContext.h"
#include "Core/Renderer/Vulkan/VulkanStagingBuffer.h"

#include <GLFW/glfw3.h>


namespace SmolEngine
{
#define DEFAULT_FENCE_TIMEOUT 100000000000

	ImGuiVulkanImpl::ImGuiVulkanImpl()
	{

	}

	ImGuiVulkanImpl::~ImGuiVulkanImpl()
	{

	}

	void ImGuiVulkanImpl::Init()
	{
		g_Device = *VulkanContext::GetDevice().GetLogicalDevice();

		// Create Descriptor Pool
		{
			VkDescriptorPoolSize pool_sizes[] =
			{
				{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
			};

			VkDescriptorPoolCreateInfo pool_info = {};
			pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
			pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
			pool_info.pPoolSizes = pool_sizes;
			VK_CHECK_RESULT(vkCreateDescriptorPool(g_Device, &pool_info, nullptr, &g_DescriptorPool));
		}

		VkPipelineCacheCreateInfo pipelineCacheCI = {};
		{
			pipelineCacheCI.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		}
		VK_CHECK_RESULT(vkCreatePipelineCache(g_Device, &pipelineCacheCI, nullptr, &g_PipelineCache));
	}

	void ImGuiVulkanImpl::Reset()
	{

	}

	void ImGuiVulkanImpl::InitResources()
	{
		VkCommandBuffer copyCmd = VulkanContext::GetCommandBuffer().CreateSingleCommandBuffer();
		{
			ImGui_ImplVulkan_CreateFontsTexture(copyCmd);
		}
		VulkanContext::GetCommandBuffer().EndSingleCommandBuffer(copyCmd);

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
}