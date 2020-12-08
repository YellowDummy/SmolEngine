#include "stdafx.h"
#include "VulkanTestLayer.h"
#include "Core/Application.h"
#include "Core/Window.h"

#include "Core/Renderer/Shader.h"
#include "Core/Renderer/Renderer2D.h"
#include "Core/Renderer/Vulkan/Vulkan.h"
#include "Core/Renderer/Vulkan/VulkanContext.h"
#include "Core/Renderer/Vulkan/VulkanPipelineSpecification.h"

#include <../Libraries/imgui/examples/imgui_impl_vulkan.h>

namespace SmolEngine
{
	void VulkanTestLayer::OnAttach()
	{
		float aspectRatio = (float)Application::GetApplication().GetWindowWidth() / (float)Application::GetApplication().GetWindowHeight();
		m_EditorCamera = std::make_shared<EditorCameraController>(aspectRatio);
		m_EditorCamera->SetZoom(4.0f);

		m_Shader = Shader::Create("../SmolEngine/Assets/Shaders/VulkanTriangle_Vertex.glsl",
			"../SmolEngine/Assets/Shaders/VulkanTriangle_Fragment.glsl");

		m_TestTexture.CreateTexture2D("../GameX/Assets/Textures/SummerBG.png");

		struct Vertex
		{
			glm::vec3 Pos;
			glm::vec4 Color;
			glm::vec2 TexCood;
		};

		Vertex verticies[4] =
		{
			// pos                       // color                    // texCood
			{ { -0.5f, 0.5, 0.0f },  { 1.0f, 0.0f, 1.0f, 1.0f },  { 0.0f, 1.0f },},
			{ { 0.5f, 0.5f, 0.0f },  { 0.2f, 1.0f, 0.0f, 1.0f },  { 1.0f, 1.0f},},
			{ {  0.5f, -0.5, 0.0f},  { 0.5f, 0.0f, 1.0f, 1.0f },  { 1.0f, 0.0f },},
			{ { -0.5f, -0.5, 0.0f },  { 1.0f, 0.0f, 1.0f, 1.0f },  { 0.0f, 0.0f } },
		};


		m_VertexBuffer.Create(verticies, sizeof(verticies));

		uint32_t indices[6] = { 0, 1, 2,  2, 3, 0 };
		m_IndexBuffer.Create(indices, sizeof(indices));

		uint32_t width = VulkanContext::GetSwapchain().GetWidth();
		uint32_t height = VulkanContext::GetSwapchain().GetHeight();

		//glm::mat4 proj = glm::perspectiveFov(45.0f, (float)width, (float)height, 0.0001f, 1000.0f);
		//glm::mat4 view = glm::translate(glm::mat4(1.0f), { 0.0f, 0.0f, -5.0f });
		//glm::mat viewProj = proj * view;
		//uint32_t bindingPoint = 0;
		//m_Shader->SumbitUniformBuffer(bindingPoint, &viewProj, sizeof(glm::mat4));

#ifndef SMOLENGINE_OPENGL_IMPL

		VulkanPipelineSpecification pipelineSpecCI = {};
		{
			pipelineSpecCI.Device = &VulkanContext::GetDevice();
			pipelineSpecCI.Shader = m_Shader->GetVulkanShader();
			pipelineSpecCI.TargetSwapchain = &VulkanContext::GetSwapchain();
			pipelineSpecCI.Texture = &m_TestTexture;
		}

		m_Pipeline.Invalidate(&pipelineSpecCI);

#endif // !SMOLENGINE_OPENGL_IMPL
	}

	void VulkanTestLayer::OnDetach()
	{

	}

	void VulkanTestLayer::OnUpdate(DeltaTime deltaTime)
	{
		m_EditorCamera->OnUpdate(deltaTime);

		uint32_t bindingPoint = 0;
		m_Shader->SumbitUniformBuffer(bindingPoint, &m_EditorCamera->GetCamera()->GetViewProjectionMatrix(), sizeof(glm::mat4));

		BuildTestCommandBuffer();
	}

	void VulkanTestLayer::OnEvent(Event& event)
	{
		m_EditorCamera->OnEvent(event);
	}

	void VulkanTestLayer::OnImGuiRender()
	{
		ImGui::Begin("Shader Settings");
		{
			if (ImGui::Button("Reload Shader"))
			{
				m_Shader->Realod();
				m_Pipeline.Destroy();

				VulkanPipelineSpecification pipelineSpecCI = {};
				{
					pipelineSpecCI.Device = &VulkanContext::GetDevice();
#ifndef SMOLENGINE_OPENGL_IMPL
					pipelineSpecCI.Shader = m_Shader->GetVulkanShader();
#endif
					pipelineSpecCI.TargetSwapchain = &VulkanContext::GetSwapchain();
				}

				m_Pipeline.Invalidate(&pipelineSpecCI);
			}


			ImGui::ColorPicker4("Add Color", glm::value_ptr(m_AddColor));

			ImGui::NewLine();

			ImGui::Image((ImTextureID)(intptr_t)m_TestTexture.m_Image, ImVec2{ 50, 50 }, ImVec2(0, 1), ImVec2(1, 0));
		}
		ImGui::End();
	}

	void VulkanTestLayer::BuildTestCommandBuffer()
	{
		auto& framebuffers = VulkanContext::GetSwapchain().GetFramebuffer().GetVkFramebuffers();
		auto drawCmdBuffers = VulkanContext::GetCommandBuffer().GetVkCommandBuffer();
		uint32_t width = VulkanContext::GetSwapchain().GetWidth();
		uint32_t height = VulkanContext::GetSwapchain().GetHeight();
		uint32_t index = VulkanContext::GetSwapchain().GetCurrentBufferIndex();

		VkCommandBufferBeginInfo cmdBufInfo = {};
		cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufInfo.pNext = nullptr;

		// Set clear values for all framebuffer attachments with loadOp set to clear
		// We use two attachments (color and depth) that are cleared at the start of the subpass and as such we need to set clear values for both
		VkClearValue clearValues[2];
		clearValues[0].color = { { 0.1f, 0.1f, 0.1f, 1.0f } };
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

		VK_CHECK_RESULT(vkBeginCommandBuffer(drawCmdBuffers, &cmdBufInfo));

		// Start the first sub pass specified in our default render pass setup by the base class
		// This will clear the color and depth attachment
		vkCmdBeginRenderPass(drawCmdBuffers, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Update dynamic viewport state
		VkViewport viewport = {};
		viewport.x = 0;
		viewport.y = (float)height;
		viewport.height = -(float)height;
		viewport.width = (float)width;
		viewport.minDepth = (float)0.0f;
		viewport.maxDepth = (float)1.0f;
		vkCmdSetViewport(drawCmdBuffers, 0, 1, &viewport);

		// Update dynamic scissor state
		VkRect2D scissor = {};
		scissor.extent.width = width;
		scissor.extent.height = height;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		vkCmdSetScissor(drawCmdBuffers, 0, 1, &scissor);

#ifndef SMOLENGINE_OPENGL_IMPL

		const auto& descriptorsSet = m_Pipeline.GetVkDescriptorSet();
		// Bind descriptor sets describing shader binding points
		vkCmdBindDescriptorSets(drawCmdBuffers, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline.GetVkPipelineLayot(), 0, 1,
			&descriptorsSet, 0, nullptr);

#endif // !SMOLENGINE_OPENGL_IMPL

		// Bind the rendering pipeline
		// The pipeline (state object) contains all states of the rendering pipeline, binding it will set all the states specified at pipeline creation time

		vkCmdBindPipeline(drawCmdBuffers, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline.GetVkPipeline());

		// Bind triangle vertex buffer (contains position and colors)

		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(drawCmdBuffers, 0, 1, &m_VertexBuffer.GetBuffer(), offsets);

		// Bind triangle index buffer
		vkCmdBindIndexBuffer(drawCmdBuffers, m_IndexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

		// Submit Push Constant

		vkCmdPushConstants(drawCmdBuffers, m_Pipeline.GetVkPipelineLayot(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(glm::vec3), &m_AddColor);

		// Draw indexed triangle
		vkCmdDrawIndexed(drawCmdBuffers, 6, 1, 0, 0, 0);

		// Draw Imgui
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), drawCmdBuffers);

		vkCmdEndRenderPass(drawCmdBuffers);

		// Ending the render pass will add an implicit barrier transitioning the frame buffer color attachment to
		// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR for presenting it to the windowing system

		VK_CHECK_RESULT(vkEndCommandBuffer(drawCmdBuffers));
	}
}