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

		FramebufferSpecification framebufferCI = {};
		framebufferCI.Width = VulkanContext::GetSwapchain().GetWidth();
		framebufferCI.Height = VulkanContext::GetSwapchain().GetHeight();
		m_FrameBuffer = Framebuffer::Create(framebufferCI);

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
		uint32_t indices[6] = { 0, 1, 2,  2, 3, 0 };
		BufferLayout layout({ 

			{ ShaderDataType::Float3, "aPos" }, // location 0
			{ ShaderDataType::Float4, "aColor" },
			{ ShaderDataType::Float2, "aTexCood" } // location 2
		});

		m_Tetxure1 = Texture2D::Create("../GameX/Assets/Textures/SummerBG.png");
		m_Tetxure2 = Texture2D::Create("../GameX/Assets/Textures/bulkhead-wallsx3.png");

		VertexBufferCreateInfo vertexBufferCI = {};
		{
			vertexBufferCI.BufferLayot = &layout;
			vertexBufferCI.Size = sizeof(verticies);
			vertexBufferCI.Vertices = &verticies;
		}

		IndexBufferCreateInfo indexBufferCI = {};
		{
			indexBufferCI.Count = 6;
			indexBufferCI.Indices = indices;
		}

		GraphicsPipelineShaderCreateInfo shaderCI = {};
		{
			shaderCI.FilePaths[ShaderType::Vertex] = "../SmolEngine/Assets/Shaders/VulkanTriangle_Vertex.glsl";
			shaderCI.FilePaths[ShaderType::Fragment] = "../SmolEngine/Assets/Shaders/VulkanTriangle_Fragment.glsl";
			shaderCI.Textures = { m_Tetxure1, m_Tetxure2 };
		}

		GraphicsPipelineCreateInfo graphicsPipelineCI = {};
		{
			graphicsPipelineCI.IndexBuffer = &indexBufferCI;
			graphicsPipelineCI.VertexBuffer = &vertexBufferCI;
			graphicsPipelineCI.ShaderCreateInfo = &shaderCI;
		}

		m_GraphicsPipeline.Create(&graphicsPipelineCI);
	}

	void VulkanTestLayer::OnDetach()
	{

	}

	void VulkanTestLayer::OnUpdate(DeltaTime deltaTime)
	{
		m_EditorCamera->OnUpdate(deltaTime);

		BuildTestCommandBuffer();
	}

	void VulkanTestLayer::OnEvent(Event& event)
	{
		m_EditorCamera->OnEvent(event);

		if (event.m_EventType == (int)EventType::S_WINDOW_RESIZE)
		{
			auto& e = static_cast<WindowResizeEvent&>(event);
			m_FrameBuffer->OnResize(e.GetWidth(), e.GetHeight());
		}
	}

	void VulkanTestLayer::OnImGuiRender()
	{
		ImGui::Begin("Shader Settings");
		{
			if (ImGui::Button("Reload Shader"))
			{

			}


			ImGui::ColorPicker4("Add Color", glm::value_ptr(m_AddColor));

			ImGui::NewLine();

			ImGui::Image(m_FrameBuffer->GetImGuiTextureID(), ImVec2{ 100, 100 });
		}
		ImGui::End();
	}

	void VulkanTestLayer::BuildTestCommandBuffer()
	{
		auto drawCmdBuffers = VulkanContext::GetCommandBuffer().GetVkCommandBuffer();

		VkCommandBufferBeginInfo cmdBufInfo = {};
		{
			cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			cmdBufInfo.pNext = nullptr;
		}

		VK_CHECK_RESULT(vkBeginCommandBuffer(drawCmdBuffers, &cmdBufInfo));

		// Fist Render Pass - Main
		{
			uint32_t bindingPoint = 0;
			m_GraphicsPipeline.SumbitUniformBuffer(bindingPoint, sizeof(glm::mat4), &m_EditorCamera->GetCamera()->GetViewProjectionMatrix());

			m_GraphicsPipeline.BeginRenderPass(m_FrameBuffer, glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
			{
				m_GraphicsPipeline.SumbitPushConstant(ShaderType::Fragment, sizeof(glm::vec3), &m_AddColor);
				m_GraphicsPipeline.DrawIndexed();
			}
			m_GraphicsPipeline.EndRenderPass();
		}

		// Second Render Pass - ImGui
		{
			auto& framebuffers = VulkanContext::GetSwapchain().GetSwapchainFramebuffer().GetVkFramebuffers();
			uint32_t width = VulkanContext::GetSwapchain().GetWidth();
			uint32_t height = VulkanContext::GetSwapchain().GetHeight();
			uint32_t index = VulkanContext::GetSwapchain().GetCurrentBufferIndex();

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

			// Draw Imgui
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), drawCmdBuffers);

			vkCmdEndRenderPass(drawCmdBuffers);
		}

		// Ending the render pass will add an implicit barrier transitioning the frame buffer color attachment to
		// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR for presenting it to the windowing system

		VK_CHECK_RESULT(vkEndCommandBuffer(drawCmdBuffers));
	}
}