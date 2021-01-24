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

#include "Renderer/Vulkan/VulkanPBR.h"

#include "ECS/Systems/CommandSystem.h"


namespace SmolEngine
{
	void PBRTestLayer::OnAttach()
	{
		VulkanPBR::Init("../Resources/Textures/gcanyon_cube.ktx", TextureFormat::R16G16B16A16_SFLOAT);

		EditorCameraCreateInfo cameraCI = {};
		{
			cameraCI.IsFramebufferTargetsSwapchain = true;
		}

		m_EditorCamera = std::make_shared<EditorCamera>(&cameraCI);
		m_FrameBuffer = m_EditorCamera->GetFramebuffer();

		m_Tetxure1 = Texture::Create("../Resources/glock_17_BaseColor.png");
		m_Tetxure3 = Texture::Create("../Resources/glock_17_Normal.png");
		m_Tetxure2 = Texture::Create("../Resources/glock_17_Metallic.png");
		m_Tetxure4 = Texture::Create("../Resources/glock_17_Roughness.png");
		m_TestMesh = Mesh::Create("../Resources/Glock_17.fbx");

		{
			m_Pipeline = std::make_shared<GraphicsPipeline>();
			GraphicsPipelineShaderCreateInfo shaderCI = {};
			{
				shaderCI.FilePaths[ShaderType::Vertex] = "../Resources/Shaders/PBR_Vulkan_Vertex.glsl";
				shaderCI.FilePaths[ShaderType::Fragment] = "../Resources/Shaders/PBR_Vulkan_Fragment.glsl";
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

			std::vector<Ref<VertexBuffer>> buffer = { m_TestMesh->m_VertexBuffer };
			m_Pipeline->SetDynamicVertexBuffers(buffer);

#ifndef SMOLENGINE_OPENGL_IMPL
			m_Pipeline->UpdateVulkanImageDescriptor(2, VulkanPBR::GetIrradianceImageInfo());
			m_Pipeline->UpdateVulkanImageDescriptor(3, VulkanPBR::GetBRDFLUTImageInfo());
			m_Pipeline->UpdateVulkanImageDescriptor(4, VulkanPBR::GetPrefilteredCubeImageInfo());
#endif

			m_Pipeline->UpdateSampler(m_Tetxure1, 5); //albedo
			m_Pipeline->UpdateSampler(m_Tetxure3, 6); //normal
			m_Pipeline->UpdateSampler(m_Tetxure1, 7); //ao
			m_Pipeline->UpdateSampler(m_Tetxure2, 8); //metallic
			m_Pipeline->UpdateSampler(m_Tetxure4, 9); //roughness
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

			for (uint32_t i = 0; i < 4; ++i)
			{
				if (i < 1)
					m_UBOParams.lights[i] = glm::vec4(-1, 1, 1, 1);
				else
					m_UBOParams.lights[i] = glm::vec4(255, 255, 255,255);

				m_UBOParams.lightsColors[i] = glm::vec4(1, 1, 1, 1);
			}

			m_UBOParams.exposure = 4;
			m_UBOParams.gamma = 2.5f;

			Ref<VertexBuffer> skyBoxFB = VertexBuffer::Create(skyboxVertices, sizeof(skyboxVertices));

			std::vector<Ref<VertexBuffer>> buffer = { skyBoxFB };
			m_SkyboxPipeline->SetDynamicVertexBuffers(buffer);
		}
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
		if(m_RorateModel)
			rot.y += 0.0005f;

		ImGui::Begin("Shader Settings");
		{

			//ImGui::NewLine();
			//ImGui::ColorPicker4("Add Color", glm::value_ptr(m_AddColor));

			ImGui::NewLine();
			ImGui::InputFloat3("Translation", glm::value_ptr(pos));
			ImGui::InputFloat3("Scale", glm::value_ptr(scale));
			ImGui::InputFloat3("Rotation", glm::value_ptr(rot));

			ImGui::Checkbox("RotateModel", &m_RorateModel);

			ImGui::NewLine();
			if (ImGui::Button("Reload Shader"))
			{
				if (m_Pipeline->Reload())
				{
#ifndef SMOLENGINE_OPENGL_IMPL
					m_Pipeline->UpdateVulkanImageDescriptor(2, VulkanPBR::GetIrradianceImageInfo());
					m_Pipeline->UpdateVulkanImageDescriptor(3, VulkanPBR::GetBRDFLUTImageInfo());
					m_Pipeline->UpdateVulkanImageDescriptor(4, VulkanPBR::GetPrefilteredCubeImageInfo());
#endif

					m_Pipeline->UpdateSampler(m_Tetxure1, 5); //albedo
					m_Pipeline->UpdateSampler(m_Tetxure3, 6); //normal
					m_Pipeline->UpdateSampler(m_Tetxure1, 7); //ao
					m_Pipeline->UpdateSampler(m_Tetxure2, 8); //metallic
					m_Pipeline->UpdateSampler(m_Tetxure4, 9); //roughness
				}
			}

			ImGui::NewLine();
			ImGui::InputFloat4("Position_3", glm::value_ptr(m_UBOParams.lights[2]));
			ImGui::InputFloat4("Position_4", glm::value_ptr(m_UBOParams.lights[3]));

			ImGui::NewLine();

			ImGui::ColorPicker3("Color_1", glm::value_ptr(m_UBOParams.lightsColors[0]));
			ImGui::ColorPicker3("Color_2", glm::value_ptr(m_UBOParams.lightsColors[1]));

			ImGui::NewLine();

			ImGui::InputFloat("Gamma", &m_UBOParams.gamma);
			ImGui::InputFloat("Exposure", &m_UBOParams.exposure);
		}
		ImGui::End();

		//ImGui::Begin("GameView");
		//ImGui::Image(m_FrameBuffer->GetImGuiTextureID(), ImVec2{ 200, 200 });
		//ImGui::End();
	}

	void PBRTestLayer::BuildTestCommandBuffer()
	{
		bool useMainCmdBuffer = true; // executed at swapbuffers() inside VulkanContex class


		// Skybox
		{
			m_SkyboxPipeline->BeginCommandBuffer(useMainCmdBuffer);
			m_SkyboxPipeline->BeginBufferSubmit();

			m_SkyboxPipeline->BeginRenderPass(m_FrameBuffer);
			{
				m_SkyboxPipeline->ClearColors();

				{
					struct PushConstant
					{
						glm::mat4 proj;
						glm::mat4 model;
						float exposure;
						float gamma;
					} pc;

					pc.proj = m_EditorCamera->GetProjection();
					pc.model = glm::inverse(m_EditorCamera->GetViewMatrix());
					pc.model[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
					pc.exposure = m_UBOParams.exposure;
					pc.gamma = m_UBOParams.gamma;

					m_SkyboxPipeline->SumbitPushConstant(ShaderType::Vertex, sizeof(PushConstant), &pc);
					m_SkyboxPipeline->Draw(36);
				}

				
			}
			m_SkyboxPipeline->EndRenderPass();
			m_SkyboxPipeline->EndBufferSubmit();
			m_SkyboxPipeline->EndCommandBuffer();
		}

		// Model
		{
			m_Pipeline->BeginCommandBuffer(useMainCmdBuffer);
			m_Pipeline->BeginBufferSubmit();

			m_Pipeline->SumbitUniformBuffer(15, sizeof(UBOParams), &m_UBOParams);

			m_Pipeline->BeginRenderPass(m_FrameBuffer);
			{
				glm::mat4 trans;
				CommandSystem::ComposeTransform(pos, rot, scale, true, trans);

				struct PushConsant
				{
					glm::mat4 proj;
					glm::mat4 model;
					glm::mat4 view;
					glm::vec3 camPos;

				} pc;

				pc.proj = m_EditorCamera->GetProjection();
				pc.view = m_EditorCamera->GetViewMatrix();
				pc.model = trans;
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