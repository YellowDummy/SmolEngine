#include "stdafx.h"
#include "PBRTestLayer.h"
#include "Core/Application.h"
#include "Core/Window.h"

#include "Renderer/Shader.h"
#include "Renderer/CubeTexture.h"
#include "Renderer/Mesh.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/Vulkan/Vulkan.h"
#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanPipelineSpecification.h"

#include "ECS/Systems/CommandSystem.h"


namespace SmolEngine
{
	static const uint32_t s_SamplersBindingPoint = 9;

	void PBRTestLayer::OnAttach()
	{
		m_CubeTexture = CubeTexture::Create("../Resources/Textures/cubemap_yokohama_rgba.ktx");

		float aspectRatio = (float)Application::GetApplication().GetWindowWidth() / (float)Application::GetApplication().GetWindowHeight();
		EditorCameraCreateInfo cameraCI = {};
		cameraCI.IsFramebufferTargetsSwapchain = true;
		m_EditorCamera = std::make_shared<EditorCamera>(&cameraCI);

		m_FrameBuffer = m_EditorCamera->GetFramebuffer();

		m_Tetxure1 = Texture::Create("../Resources/AK_103_Base_Color.png");
		m_Tetxure2 = Texture::Create("../Resources/AK_103_Metallic.png");
		m_Tetxure3 = Texture::Create("../Resources/AK_103_Normal.png");
		m_Tetxure4 = Texture::Create("../Resources/AK_103_Roughness.png");

		m_TestMesh = Mesh::Create("../Resources/AK-103.fbx");

		{
			m_Pipeline = std::make_shared<GraphicsPipeline>();
			GraphicsPipelineShaderCreateInfo shaderCI = {};
			{
				shaderCI.FilePaths[ShaderType::Vertex] = "../Resources/Shaders/BaseShader3D_Vulkan_Vertex.glsl";
				shaderCI.FilePaths[ShaderType::Fragment] = "../Resources/Shaders/BaseShader3D_Vulkan_Fragment.glsl";
				shaderCI.Textures = {  };
			};

			DynamicGraphicsPipelineCreateInfo DynamicPipelineCI = {};
			{
				DynamicPipelineCI.BufferLayot = &m_TestMesh->m_Layout;
				DynamicPipelineCI.PipelineName = "PBR_TEST";
				DynamicPipelineCI.Stride = m_TestMesh->m_Stride;
				DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
				DynamicPipelineCI.IsTargetsSwapchain = true;
				DynamicPipelineCI.IsDepthTestEnabled = true;
			}

			bool result = m_Pipeline->Create(&DynamicPipelineCI);
			assert(result == true);

			m_Pipeline->Update2DTextures({ m_Tetxure1, m_Tetxure2, m_Tetxure3, m_Tetxure4 }, s_SamplersBindingPoint);
			std::vector<Ref<VertexBuffer>> buffer = { m_TestMesh->m_VertexBuffer };
			m_Pipeline->SetDynamicVertexBuffers(buffer);
		}
		
		{
			m_SkyboxPipeline = std::make_shared<GraphicsPipeline>();
			GraphicsPipelineShaderCreateInfo shaderCI = {};
			{
				shaderCI.FilePaths[ShaderType::Vertex] = "../Resources/Shaders/Skybox_Vulkan_Vertex.glsl";
				shaderCI.FilePaths[ShaderType::Fragment] = "../Resources/Shaders/Skybox_Vulkan_Frag.glsl";
				shaderCI.Textures = {  };
			};

			struct SkyBoxData
			{
				glm::vec3 pos;
			};

			BufferLayout laypot =
			{
				{ ShaderDataType::Float3, "aPos" }
			};

			DynamicGraphicsPipelineCreateInfo DynamicPipelineCI = {};
			{
				DynamicPipelineCI.BufferLayot = &laypot;
				DynamicPipelineCI.PipelineName = "Skybox_Test";
				DynamicPipelineCI.Stride = sizeof(SkyBoxData);
				DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
				DynamicPipelineCI.IsTargetsSwapchain = true;
				DynamicPipelineCI.SkyBox = m_CubeTexture;
				DynamicPipelineCI.IsDepthTestEnabled = false;
			}

			bool result = m_SkyboxPipeline->Create(&DynamicPipelineCI);
			assert(result == true);

			float skyboxVertices[] = {
				// positions          
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				-1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f
			};

			Ref<VertexBuffer> skyBoxFB = VertexBuffer::Create(skyboxVertices, sizeof(skyboxVertices));

			std::vector<Ref<VertexBuffer>> buffer = { skyBoxFB };
			m_SkyboxPipeline->SetDynamicVertexBuffers(buffer);
		}


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
		rot.y += 0.0005f;

		ImGui::Begin("Shader Settings");
		{

			//ImGui::NewLine();
			//ImGui::ColorPicker4("Add Color", glm::value_ptr(m_AddColor));

			ImGui::NewLine();
			ImGui::InputFloat3("Translation", glm::value_ptr(pos));
			ImGui::InputFloat3("Scale", glm::value_ptr(scale));
			ImGui::InputFloat3("Rotation", glm::value_ptr(rot));

			ImGui::NewLine();
			if (ImGui::Button("Reload Shader"))
			{
				if (m_Pipeline->Reload())
				{
					m_Pipeline->Update2DTextures({ m_Tetxure1, m_Tetxure2, m_Tetxure3, m_Tetxure4 }, s_SamplersBindingPoint);
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
		}
		ImGui::End();

		//ImGui::Begin("GameView");
		//ImGui::Image(m_FrameBuffer->GetImGuiTextureID(), ImVec2{ 200, 200 });
		//ImGui::End();
	}

	void PBRTestLayer::BuildTestCommandBuffer()
	{
		bool useMainCmdBuffer = true; // executed at swapbuffers() inside VulkanContex class


		// Skybox pass - main command buffer
		{
			m_Pipeline->BeginCommandBuffer(useMainCmdBuffer);
			m_Pipeline->BeginBufferSubmit();

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
			m_Pipeline->SumbitUniformBuffer(10, sizeof(data), &data);

			m_SkyboxPipeline->BeginCommandBuffer(useMainCmdBuffer);
			m_SkyboxPipeline->BeginBufferSubmit();

			m_SkyboxPipeline->BeginRenderPass(m_FrameBuffer);
			{
				//m_SkyboxPipeline->ClearColors();


				{
					struct PushConstant
					{
						glm::mat4 proj;
						glm::mat4 model;
					} pc;

					pc.proj = m_EditorCamera->GetProjection();
					pc.model = glm::inverse(m_EditorCamera->GetViewMatrix());
					pc.model[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

					m_SkyboxPipeline->SumbitPushConstant(ShaderType::Vertex, sizeof(PushConstant), &pc);
					m_SkyboxPipeline->Draw(36);
				}

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

					m_Pipeline->SumbitPushConstant(ShaderType::Vertex, sizeof(PushConsant), &pc);
					m_Pipeline->Draw(m_TestMesh->m_VertexCount);
				}
			}
			m_SkyboxPipeline->EndRenderPass();
			m_SkyboxPipeline->EndBufferSubmit();
			m_Pipeline->EndBufferSubmit();
			m_SkyboxPipeline->EndCommandBuffer();
		}

		return;
		
		// Geometry pass
		{
			m_Pipeline->BeginCommandBuffer(useMainCmdBuffer);
			m_Pipeline->BeginBufferSubmit();

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

			m_Pipeline->SumbitUniformBuffer(10, sizeof(data), &data);

			m_Pipeline->BeginRenderPass(m_FrameBuffer);
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

				m_Pipeline->SumbitPushConstant(ShaderType::Vertex, sizeof(PushConsant), &pc);
				m_Pipeline->Draw(m_TestMesh->m_VertexCount);
			}
			m_Pipeline->EndRenderPass();
			m_Pipeline->EndBufferSubmit();
			m_Pipeline->EndCommandBuffer();
		}
		
	}
}