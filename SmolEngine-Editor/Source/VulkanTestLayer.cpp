#include "stdafx.h"
#include "VulkanTestLayer.h"
#include "Core/Application.h"
#include "Core/Window.h"

#include "Renderer/Shader.h"
#include "Renderer/Mesh.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/Vulkan/Vulkan.h"
#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanPipelineSpecification.h"

#include "ECS/Systems/CommandSystem.h"


namespace SmolEngine
{
	void VulkanTestLayer::OnAttach()
	{
		float aspectRatio = (float)Application::GetApplication().GetWindowWidth() / (float)Application::GetApplication().GetWindowHeight();
		m_EditorCamera = std::make_shared<EditorCamera>(45.0f, aspectRatio, 0.01f, 1000.0f);

		FramebufferSpecification framebufferCI = {};
		framebufferCI.Width = VulkanContext::GetSwapchain().GetWidth();
		framebufferCI.Height = VulkanContext::GetSwapchain().GetHeight();
		m_FrameBuffer = Framebuffer::Create(framebufferCI);

		m_Tetxure1 = Texture2D::Create("../Resources/AK_103_Base_Color.png");
		m_Tetxure2 = Texture2D::Create("../Resources/AK_103_Metallic.png");
		m_Tetxure3 = Texture2D::Create("../Resources/AK_103_Normal.png");
		m_Tetxure4 = Texture2D::Create("../Resources/AK_103_Roughness.png");

		m_TestMesh = std::make_shared<Mesh>();
		bool result = Mesh::Create("../Resources/AK-103.fbx", m_TestMesh);
		m_TestMesh->m_Pipeline->Update2DTextures({ m_Tetxure1, m_Tetxure2, m_Tetxure3, m_Tetxure4});

		for (uint32_t i = 0; i < 4; ++i)
		{
			m_PBR.lightPositions[i] = glm::vec3(i - 0.5f, i - 0.5f, -1);
			m_PBR.lightColor[i] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		}
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
			rot.y += 0.0005f;

			ImGui::NewLine();
			ImGui::ColorPicker4("Add Color", glm::value_ptr(m_AddColor));

			ImGui::NewLine();
			ImGui::InputFloat3("Translation", glm::value_ptr(pos));
			ImGui::InputFloat3("Scale", glm::value_ptr(scale));
			ImGui::InputFloat3("Rotation", glm::value_ptr(rot));

			ImGui::NewLine();
			if (ImGui::Button("Reload Shader"))
			{
				if (m_TestMesh->m_Pipeline->Reload())
				{
					m_TestMesh->m_Pipeline->Update2DTextures({ m_Tetxure1, m_Tetxure2, m_Tetxure3, m_Tetxure4 });
				}
			}
			ImGui::NewLine();
			//ImGui::Image(m_FrameBuffer->GetImGuiTextureID(), ImVec2{ 200, 200 });
		}
		ImGui::End();
	}

	void VulkanTestLayer::BuildTestCommandBuffer()
	{
		m_TestMesh->m_Pipeline->BeginCommandBuffer(true);
		m_TestMesh->m_Pipeline->BeginBufferSubmit();

		m_TestMesh->m_Pipeline->BeginRenderPass();
		{
			glm::mat4 trans;
			CommandSystem::ComposeTransform(pos, rot, scale, true, trans);

			struct PushConsant
			{
				glm::mat4 viewProj;
				glm::mat4 trans;
				glm::vec4 color;
				glm::vec3 camPos;

			} pc;

			pc.viewProj = m_EditorCamera->GetViewProjection();
			pc.trans = trans;
			pc.color = m_AddColor;
			pc.camPos = m_EditorCamera->GetPosition();

			m_TestMesh->m_Pipeline->ClearColors();
			m_TestMesh->m_Pipeline->SumbitPushConstant(ShaderType::Vertex, sizeof(PushConsant), &pc);
			m_TestMesh->m_Pipeline->Draw(m_TestMesh->m_VertexCount);
		}
		m_TestMesh->m_Pipeline->EndRenderPass();
		m_TestMesh->m_Pipeline->EndBufferSubmit();
		m_TestMesh->m_Pipeline->EndCommandBuffer();
#if 0
		m_GraphicsPipeline.BeginCommandBuffer();

		{
			m_GraphicsPipeline.Update2DTextures({ m_Tetxure1 });
			{
				uint32_t bindingPoint = 0;
				m_GraphicsPipeline.SumbitUniformBuffer(bindingPoint, sizeof(glm::mat4), &m_EditorCamera->GetCamera()->GetViewProjectionMatrix());

				m_GraphicsPipeline.BeginRenderPass(m_FrameBuffer);
				{
					m_GraphicsPipeline.ClearColors(m_FrameBuffer);
					m_GraphicsPipeline.SumbitPushConstant(ShaderType::Fragment, sizeof(glm::vec3), &m_AddColor);
					m_GraphicsPipeline.DrawIndexed();
				}
				m_GraphicsPipeline.EndRenderPass();
			}

		}
		m_GraphicsPipeline.EndCommandBuffer();

#endif


	}
}