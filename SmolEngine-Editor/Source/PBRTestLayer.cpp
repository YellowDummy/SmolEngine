#include "stdafx.h"
#include "PBRTestLayer.h"
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
	void PBRTestLayer::OnAttach()
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

		m_Light.Pos = { 0, 1, -2, 1 };
		m_Light.Color = { 0.2, 0.7, 0.5, 1.0f };

		m_Light2.Pos = { 0, 0.5f, 1, 1 };
		m_Light2.Color = { 0.8, 0.4, 0.3, 1.0f };
	}

	void PBRTestLayer::OnDetach()
	{

	}

	void PBRTestLayer::OnUpdate(DeltaTime deltaTime)
	{
		m_EditorCamera->OnUpdate(deltaTime);

		BuildTestCommandBuffer();
	}

	void PBRTestLayer::OnEvent(Event& event)
	{
		m_EditorCamera->OnEvent(event);

		if (event.m_EventType == (int)EventType::S_WINDOW_RESIZE)
		{
			auto& e = static_cast<WindowResizeEvent&>(event);
			m_FrameBuffer->OnResize(e.GetWidth(), e.GetHeight());
		}
	}

	void PBRTestLayer::OnImGuiRender()
	{
		ImGui::Begin("Shader Settings");
		{
			rot.y += 0.0005f;

			//ImGui::NewLine();
			//ImGui::ColorPicker4("Add Color", glm::value_ptr(m_AddColor));

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

			ImGui::ColorPicker4("LightColor", glm::value_ptr(m_Light.Color));
			ImGui::InputFloat4("Position", glm::value_ptr(m_Light.Pos));
			ImGui::InputFloat("Intensity", &m_Light.intensity);

			ImGui::NewLine();
			ImGui::ColorPicker4("LightColor_2", glm::value_ptr(m_Light2.Color));
			ImGui::InputFloat4("Position_2", glm::value_ptr(m_Light2.Pos));
			ImGui::InputFloat("Intensity_2", &m_Light2.intensity);

			//ImGui::Image(m_FrameBuffer->GetImGuiTextureID(), ImVec2{ 200, 200 });
		}
		ImGui::End();
	}

	void PBRTestLayer::BuildTestCommandBuffer()
	{
		m_TestMesh->m_Pipeline->BeginCommandBuffer(true);
		m_TestMesh->m_Pipeline->BeginBufferSubmit();

		struct Data
		{
			glm::vec4 color;
			glm::vec4 pos;
			glm::vec4 atr;

		} data[2];

		data[0].color = m_Light.Color;
		data[0].pos = m_Light.Pos;
		data[0].atr.r = m_Light.intensity;
		data[1].color = m_Light2.Color;
		data[1].pos = m_Light2.Pos;
		data[1].atr.r = m_Light2.intensity;

		m_TestMesh->m_Pipeline->SumbitUniformBuffer(10, sizeof(data), &data);

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