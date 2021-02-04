#include "stdafx.h"
#include "DeferredRenderingTest.h"
#include "Core/Application.h"
#include "Core/Window.h"

#include "Renderer/Shader.h"
#include "Renderer/CubeTexture.h"
#include "Renderer/Mesh.h"
#include "Renderer/SharedUtils.h"

#include "Renderer/Vulkan/Vulkan.h"
#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanPipelineSpecification.h"

#include "Renderer/Vulkan/VulkanPBR.h"

#include "ECS/Systems/CommandSystem.h"

namespace SmolEngine
{
	void DeferredRenderingTest::OnAttach()
	{
		VulkanPBR::Init("../Resources/Textures/gcanyon_cube.ktx", TextureFormat::R16G16B16A16_SFLOAT);

		FramebufferSpecification framebefferSpec = {};
		framebefferSpec.IsUseMRT = true;
		m_DeferredFrameBuffer = Framebuffer::Create(framebefferSpec);

		m_Tetxure1 = Texture::Create("../Resources/SMGtextureSet_Base_Color.png");
		m_Tetxure3 = Texture::Create("../Resources/SMGtextureSet_Normal_DirectX.png");
		m_Tetxure2 = Texture::Create("../Resources/SMGtextureSet_Metallic.png");
		m_Tetxure4 = Texture::Create("../Resources/SMGtextureSet_Roughness.png");
		m_Tetxure5 = Texture::Create("../Resources/SMGtextureSet_AO.png");

		m_TestMesh = Mesh::Create("../Resources/30_SMG_LP.obj");

		{
			m_Pipeline = std::make_shared<GraphicsPipeline>();
			GraphicsPipelineShaderCreateInfo shaderCI = {};
			{
				shaderCI.FilePaths[ShaderType::Vertex] = "../Resources/Shaders/MRT_Vulkan_Vertex.glsl";
				shaderCI.FilePaths[ShaderType::Fragment] = "../Resources/Shaders/MRT_Vulkan_Fragment.glsl";
			};

			BufferLayout mainLayout =
			{
				{ ShaderDataType::Float3, "aPos" },
				{ ShaderDataType::Float3, "aNormal" },
				{ ShaderDataType::Float4, "aTangent" },
				{ ShaderDataType::Float2, "aUV" },
				{ ShaderDataType::Float4, "aColor" }
			};

			GraphicsPipelineCreateInfo DynamicPipelineCI = {};
			{
				DynamicPipelineCI.VertexInputInfos = { VertexInputInfo(sizeof(PBRVertex), mainLayout) };
				DynamicPipelineCI.PipelineName = "Deferred_Rendering";
				DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
				DynamicPipelineCI.IsUseMRT = true;
			}

			bool result = m_Pipeline->Create(&DynamicPipelineCI);
			assert(result == true);

			m_Pipeline->SetVertexBuffers({ m_TestMesh->GetVertexBuffer() });
			m_Pipeline->SetIndexBuffers({ m_TestMesh->GetIndexBuffer() });

			m_Pipeline->UpdateSampler(m_Tetxure1, 5); //albedo
			m_Pipeline->UpdateSampler(m_Tetxure3, 6); //normal
			m_Pipeline->UpdateSampler(m_Tetxure5, 7); //ao
			m_Pipeline->UpdateSampler(m_Tetxure2, 8); //metallic
			m_Pipeline->UpdateSampler(m_Tetxure4, 9); //roughness
		}

		// Combination Pipeline
		{
			m_CombinationPipeline = std::make_shared<GraphicsPipeline>();

			GraphicsPipelineShaderCreateInfo shaderCI = {};
			{
				shaderCI.FilePaths[ShaderType::Vertex] = "../Resources/Shaders/PBR_MRT_Vulkan_Vertex.glsl";
				shaderCI.FilePaths[ShaderType::Fragment] = "../Resources/Shaders/PBR_MRT_Vulkan_Fragment.glsl";
			};

			GraphicsPipelineCreateInfo DynamicPipelineCI = {};
			{
				DynamicPipelineCI.VertexInputInfos = {  };
				DynamicPipelineCI.PipelineName = "Deferred_Rendering_Combination";
				DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
				DynamicPipelineCI.IsTargetsSwapchain = true;
			}

			bool result = m_CombinationPipeline->Create(&DynamicPipelineCI);
			assert(result == true);

			EditorCameraCreateInfo cameraCI = {};
			{
				cameraCI.IsFramebufferTargetsSwapchain = true;
			}

			m_EditorCamera = std::make_shared<EditorCamera>(&cameraCI);

#ifndef SMOLENGINE_OPENGL_IMPL

			auto& pass = m_DeferredFrameBuffer->GetVulkanFramebuffer().GetDeferredPass();

			m_CombinationPipeline->UpdateVulkanImageDescriptor(1, pass.positionImageInfo);
			m_CombinationPipeline->UpdateVulkanImageDescriptor(2, pass.normalsImageInfo);
			m_CombinationPipeline->UpdateVulkanImageDescriptor(3, pass.colorImageInfo);
			m_CombinationPipeline->UpdateVulkanImageDescriptor(4, pass.pbrImageInfo);


			m_CombinationPipeline->UpdateVulkanImageDescriptor(5, VulkanPBR::GetIrradianceImageInfo());
			m_CombinationPipeline->UpdateVulkanImageDescriptor(6, VulkanPBR::GetBRDFLUTImageInfo());
			m_CombinationPipeline->UpdateVulkanImageDescriptor(7, VulkanPBR::GetPrefilteredCubeImageInfo());
#endif

		}
	}

	void DeferredRenderingTest::OnDetach()
	{

	}

	void DeferredRenderingTest::OnEvent(Event& event)
	{
		m_EditorCamera->OnEvent(event);
	}

	void DeferredRenderingTest::OnImGuiRender()
	{
		if (m_RorateModel)
			m_Rot.y += 0.0005f;

		ImGui::Begin("SomeWIndow");
		ImGui::Checkbox("RotateModel", &m_RorateModel);
		ImGui::End();
	}

	void DeferredRenderingTest::OnUpdate(DeltaTime deltaTime)
	{
		m_EditorCamera->OnUpdate(deltaTime);

		BuildTestCommandBuffer();
	}

	void DeferredRenderingTest::BuildTestCommandBuffer()
	{
		//Offscreen
		{
			m_Pipeline->BeginCommandBuffer();
			m_Pipeline->BeginBufferSubmit();

			m_Pipeline->BeginRenderPass(m_DeferredFrameBuffer);
			{
				glm::mat4 trans;
				CommandSystem::ComposeTransform(m_Pos, m_Rot, m_Scale, true, trans);

				struct PushConsant
				{
					glm::mat4 proj;
					glm::mat4 model;
					glm::mat4 view;

				} pc;

				pc.proj = m_EditorCamera->GetProjection();
				pc.view = m_EditorCamera->GetViewMatrix();
				pc.model = trans;

				m_Pipeline->SumbitPushConstant(ShaderType::Vertex, sizeof(PushConsant), &pc);
				m_Pipeline->DrawIndexed();
			}
			m_Pipeline->EndRenderPass();

			m_Pipeline->EndBufferSubmit();
			m_Pipeline->EndCommandBuffer();
		}

		//Final
		{
			m_CombinationPipeline->BeginCommandBuffer(true);
			m_CombinationPipeline->BeginBufferSubmit();

			m_Params.viewPos = glm::vec4(m_EditorCamera->GetPosition(), 1);

			m_CombinationPipeline->SumbitUniformBuffer(15, sizeof(UBOMRTParams), &m_Params);

			m_CombinationPipeline->BeginRenderPass(m_EditorCamera->GetFramebuffer());
			{
				m_CombinationPipeline->ClearColors();

				m_CombinationPipeline->Draw(3, DrawMode::Triangle, 0, 0, true);
			}
			m_CombinationPipeline->EndRenderPass();
			m_CombinationPipeline->EndBufferSubmit();
			m_CombinationPipeline->EndCommandBuffer();
		}
		
	}
}