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

	VkCommandBuffer ImGuiVulkanImpl::BeginSingleTimeCommand()
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = *VulkanContext::GetCommandPool().GetCommandPool();
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(*VulkanContext::GetDevice().GetLogicalDevice(), &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void ImGuiVulkanImpl::InitResources()
	{
		auto tempCommandBuffer = BeginSingleTimeCommand();
		ImGui_ImplVulkan_CreateFontsTexture(tempCommandBuffer);
		// End Command Buffer
		{
			vkEndCommandBuffer(tempCommandBuffer);

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &tempCommandBuffer;

			VkQueue q = *VulkanContext::GetDevice().GetQueue();
			vkQueueSubmit(q, 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(q);
			vkFreeCommandBuffers(*VulkanContext::GetDevice().GetLogicalDevice(), *VulkanContext::GetCommandPool().GetCommandPool(), 1, &tempCommandBuffer);
		}


		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
}