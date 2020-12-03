#include "stdafx.h"
#include "VulkanTestLayer.h"
#include "Core/Application.h"
#include "Core/Window.h"

#include "Core/Renderer/Shader.h"
#include "Core/Renderer/Renderer2D.h"
#include "Core/Renderer/Vulkan/Vulkan.h"
#include "Core/Renderer/Vulkan/VulkanContext.h"

namespace SmolEngine
{
	void VulkanTestLayer::OnAttach()
	{
		m_Shader = Shader::Create("../SmolEngine/Assets/Shaders/VulkanTriangle_Vertex.glsl",
			"../SmolEngine/Assets/Shaders/VulkanTriangle_Fragment.glsl");

		std::vector<VulkanShader*> shaders = { m_Shader->GetVulkanShader() };

		m_Pipeline.Invalidate(&VulkanContext::GetSwapchain(), &VulkanContext::GetDevice(), shaders);
	}

	void VulkanTestLayer::OnDetach()
	{

	}

	void VulkanTestLayer::OnUpdate(DeltaTime deltaTime)
	{
		BuildTestCommandBuffer();
	}

	void VulkanTestLayer::OnEvent(Event& event)
	{

	}

	void VulkanTestLayer::OnImGuiRender()
	{

	}

	void VulkanTestLayer::BuildTestCommandBuffer()
	{
		auto& framebuffers = VulkanContext::GetSwapchain().GetFramebuffer().GetVkFramebuffers();
		auto& drawCmdBuffers = VulkanContext::GetCommandBuffer().GetCommandBuffer();
		uint32_t width = VulkanContext::GetSwapchain().GetWidth();
		uint32_t height = VulkanContext::GetSwapchain().GetHeight();
		uint32_t index = VulkanContext::GetSwapchain().GetCurrentBufferIndex();

		VkCommandBufferBeginInfo cmdBufInfo = {};
		cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufInfo.pNext = nullptr;

		// Set clear values for all framebuffer attachments with loadOp set to clear
		// We use two attachments (color and depth) that are cleared at the start of the subpass and as such we need to set clear values for both
		VkClearValue clearValues[2];
		clearValues[0].color = { { 1.0f, 0.5f, 0.2f, 1.0f } };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = nullptr;
		renderPassBeginInfo.renderPass = VulkanContext::GetSwapchain().GetRenderPass();
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent.width = width;
		renderPassBeginInfo.renderArea.extent.height = height;
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;

		// Set target frame buffer

		renderPassBeginInfo.framebuffer = framebuffers[index];

		VK_CHECK_RESULT(vkBeginCommandBuffer(drawCmdBuffers[index], &cmdBufInfo));

		// Start the first sub pass specified in our default render pass setup by the base class
		// This will clear the color and depth attachment
		vkCmdBeginRenderPass(drawCmdBuffers[index], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Update dynamic viewport state
		VkViewport viewport = {};
		viewport.height = (float)height;
		viewport.width = (float)width;
		viewport.minDepth = (float)0.0f;
		viewport.maxDepth = (float)1.0f;
		vkCmdSetViewport(drawCmdBuffers[index], 0, 1, &viewport);

		// Update dynamic scissor state
		VkRect2D scissor = {};
		scissor.extent.width = width;
		scissor.extent.height = height;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		vkCmdSetScissor(drawCmdBuffers[index], 0, 1, &scissor);

		// Bind descriptor sets describing shader binding points
		//vkCmdBindDescriptorSets(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

		// Bind the rendering pipeline
		// The pipeline (state object) contains all states of the rendering pipeline, binding it will set all the states specified at pipeline creation time

		vkCmdBindPipeline(drawCmdBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline.GetVkPipeline());

		// Bind triangle vertex buffer (contains position and colors)
		//VkDeviceSize offsets[1] = { 0 };
		//vkCmdBindVertexBuffers(drawCmdBuffers[i], 0, 1, &vertices.buffer, offsets);

		// Bind triangle index buffer
		//vkCmdBindIndexBuffer(drawCmdBuffers[i], indices.buffer, 0, VK_INDEX_TYPE_UINT32);

		// Draw indexed triangle
		//vkCmdDrawIndexed(drawCmdBuffers[i], indices.count, 1, 0, 0, 1);

		vkCmdEndRenderPass(drawCmdBuffers[index]);

		// Ending the render pass will add an implicit barrier transitioning the frame buffer color attachment to
		// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR for presenting it to the windowing system

		VK_CHECK_RESULT(vkEndCommandBuffer(drawCmdBuffers[index]));
	}
}