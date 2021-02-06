#include "stdafx.h"
#include "DeferredRenderingTestLayer.h"
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
#ifdef SMOLENGINE_OPENL_IMPL // Vulkan support only
		return;
#endif

		VulkanPBR::Init("../Resources/Textures/gcanyon_cube.ktx", TextureFormat::R16G16B16A16_SFLOAT);

		EditorCameraCreateInfo cameraCI = {};
		{
			cameraCI.IsFramebufferTargetsSwapchain = true;
		}

		m_EditorCamera = std::make_shared<EditorCamera>(&cameraCI);

		FramebufferSpecification MRTframebefferSpec = {};
		MRTframebefferSpec.IsUseMRT = true;
		m_DeferredFrameBuffer = Framebuffer::Create(MRTframebefferSpec);

		FramebufferSpecification SkyBoxFramebuffer = {};
		SkyBoxFramebuffer.Height = 2048;
		SkyBoxFramebuffer.Width = 2048;
		SkyBoxFramebuffer.IsUseMSAA = false;
		m_SkyboxFrameBuffer = Framebuffer::Create(SkyBoxFramebuffer);

		m_Tetxure1 = Texture::Create("../Resources/SMGtextureSet_Base_Color.png");
		m_Tetxure3 = Texture::Create("../Resources/SMGtextureSet_Normal_DirectX.png");
		m_Tetxure2 = Texture::Create("../Resources/SMGtextureSet_Metallic.png");
		m_Tetxure4 = Texture::Create("../Resources/SMGtextureSet_Roughness.png");
		m_Tetxure5 = Texture::Create("../Resources/SMGtextureSet_AO.png");

		m_TestMesh = Mesh::Create("../Resources/30_SMG_LP.obj");

		// MRT
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

		// Skybox
		{
			m_SkyboxPipeline = std::make_shared<GraphicsPipeline>();
			GraphicsPipelineShaderCreateInfo shaderCI = {};
			{
				shaderCI.FilePaths[ShaderType::Vertex] = "../Resources/Shaders/Skybox_Vulkan_Vertex.glsl";
				shaderCI.FilePaths[ShaderType::Fragment] = "../Resources/Shaders/Skybox_Vulkan_Frag.glsl";
			};

			struct SkyBoxData
			{
				glm::vec3 pos;
			};

			BufferLayout layout =
			{
				{ ShaderDataType::Float3, "aPos" }
			};

			GraphicsPipelineCreateInfo DynamicPipelineCI = {};
			{
				DynamicPipelineCI.VertexInputInfos = { VertexInputInfo(sizeof(SkyBoxData), layout) };
				DynamicPipelineCI.PipelineName = "Skybox_Test";
				DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
				DynamicPipelineCI.IsDepthTestEnabled = false;
				DynamicPipelineCI.IsUseMSAA = false;
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
			m_SkyboxPipeline->SetVertexBuffers({ skyBoxFB });
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
				DynamicPipelineCI.PipelineDrawModes = { DrawMode::Screen };
				DynamicPipelineCI.PipelineName = "Deferred_Rendering_Combination";
				DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
				DynamicPipelineCI.IsTargetsSwapchain = true;
			}

			bool result = m_CombinationPipeline->Create(&DynamicPipelineCI);
			assert(result == true);

#ifndef SMOLENGINE_OPENGL_IMPL

			auto& pass = m_DeferredFrameBuffer->GetVulkanFramebuffer().GetDeferredPass();

			m_CombinationPipeline->UpdateVulkanImageDescriptor(0, m_SkyboxFrameBuffer->GetVulkanFramebuffer().GetOffscreenPass().colorImageInfo);
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

		ImGui::Begin("Settings");
		ImGui::Checkbox("Rotate Model", &m_RorateModel);

		ImGui::NewLine();
		std::vector<const char*> charitems = { "Final composition", "Position", "Normals", "Albedo", "Specular", "PBRParams" };
		uint32_t itemCount = static_cast<uint32_t>(charitems.size());
		bool res = ImGui::Combo("Display", &m_DrawMode, &charitems[0], itemCount, itemCount);

		ImGui::End();
	}

	void DeferredRenderingTest::OnUpdate(DeltaTime deltaTime)
	{
		m_EditorCamera->OnUpdate(deltaTime);

		BuildTestCommandBuffer();
	}

	void DeferredRenderingTest::BuildTestCommandBuffer()
	{
		// Offscreen MRT + SkyBox
		{
			VkCommandBuffer cmdBuffer;

			// MRT
			{
				m_Pipeline->BeginCommandBuffer();
				m_Pipeline->BeginBufferSubmit();

				m_Pipeline->BeginRenderPass(m_DeferredFrameBuffer);
#ifndef SMOLENGINE_OPENGL_IMPL
				cmdBuffer = m_Pipeline->GetVkCommandBuffer();
#endif
				{
					glm::mat4 trans;
					CommandSystem::ComposeTransform(m_Pos, m_Rot, m_Scale, true, trans);

					struct PushConsant
					{
						glm::mat4 proj;
						glm::mat4 model;
						glm::mat4 view;

						float nearPlane;
						float farPlane;

					} pc;

					pc.proj = m_EditorCamera->GetProjection();
					pc.view = m_EditorCamera->GetViewMatrix();
					pc.nearPlane = m_EditorCamera->GetNearClip();
					pc.farPlane = m_EditorCamera->GetFarClip();
					pc.model = trans;

					m_Pipeline->SumbitPushConstant(ShaderType::Vertex, sizeof(PushConsant), &pc);
					m_Pipeline->DrawIndexed();
				}
				m_Pipeline->EndRenderPass();

				m_Pipeline->EndBufferSubmit();
			}
	
			// Skybox
			{
#ifndef SMOLENGINE_OPENGL_IMPL
				m_SkyboxPipeline->SetCommandBuffer(cmdBuffer);
#endif
				m_SkyboxPipeline->BeginBufferSubmit();

				m_SkyboxPipeline->BeginRenderPass(m_SkyboxFrameBuffer);
				{
					m_SkyboxPipeline->ClearColors();

					struct PushConstant
					{
						glm::mat4 proj;
						glm::mat4 model;
						float exposure;
						float gamma;
					} pc;

					pc.proj = m_EditorCamera->GetProjection();
					pc.model = glm::mat4(glm::mat3(m_EditorCamera->GetViewMatrix()));
					pc.exposure = 4;
					pc.gamma = 2.5f;

					m_SkyboxPipeline->SumbitPushConstant(ShaderType::Vertex, sizeof(PushConstant), &pc);
					m_SkyboxPipeline->Draw(36);
				}
				m_SkyboxPipeline->EndRenderPass();
				m_SkyboxPipeline->EndBufferSubmit();
			}


			m_Pipeline->EndCommandBuffer(); // Submit work and wait 
		}

		// Final + Lighting
		{
			m_CombinationPipeline->BeginCommandBuffer(true);
			m_CombinationPipeline->BeginBufferSubmit();

			m_Params.viewPos = glm::vec4(m_EditorCamera->GetPosition(), 0);
			m_Params.mode = m_DrawMode;

			m_CombinationPipeline->SumbitUniformBuffer(15, sizeof(UBOMRTParams), &m_Params);

			m_CombinationPipeline->BeginRenderPass(m_EditorCamera->GetFramebuffer(), false);
			{
				m_CombinationPipeline->ClearColors();

				m_CombinationPipeline->Draw(3, DrawMode::Screen);
			}
			m_CombinationPipeline->EndRenderPass();
			m_CombinationPipeline->EndBufferSubmit();
			m_CombinationPipeline->EndCommandBuffer();
		}
		
	}
}