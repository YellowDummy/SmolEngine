#include "stdafx.h"
#include "DeferredRenderingTestLayer.h"
#include "Core/Application.h"
#include "Core/Window.h"

#include "Renderer/Shader.h"
#include "Renderer/CubeTexture.h"
#include "Renderer/Mesh.h"

#include "Renderer/Vulkan/Vulkan.h"
#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanPipelineSpecification.h"
#include "Renderer/SSAOGenerator.h"

#include "Renderer/Vulkan/VulkanPBR.h"

#include "ECS/Systems/CommandSystem.h"

namespace SmolEngine
{
	void DeferredRenderingTest::OnAttach()
	{
#ifdef SMOLENGINE_OPENL_IMPL // Vulkan support only
		return;
#endif
		// Instance Data

		PBRVertexInstanced instanceOne = {};
		{
			instanceOne.UseAlbedroMap = true;
			instanceOne.UseAOMap = true;
			instanceOne.UseMetallicMap = true;
			instanceOne.UseNormalMap = true;
			instanceOne.UseRoughnessMap = true;

			instanceOne.AlbedroMapIndex = 0;
			instanceOne.AOMapIndex = 0;
			instanceOne.MetallicMapIndex = 0;
			instanceOne.NormalMapIndex = 0;
			instanceOne.RoughnessMapIndex = 0;
		}

		PBRVertexInstanced instanceTwo = {};
		{
			instanceTwo.UseAlbedroMap = false;
			instanceTwo.UseAOMap = false;
			instanceTwo.UseMetallicMap = false;
			instanceTwo.UseNormalMap = false;
			instanceTwo.UseRoughnessMap = false;

			instanceTwo.Roughness = 0.1f;
			instanceTwo.Metallic = 0.2f;
		}

		BufferLayout PBRInstanceLayout =
		{
			{ ShaderDataType::Int, "useAlbedroMap" },
			{ ShaderDataType::Int, "useNormalMap" },
			{ ShaderDataType::Int, "useMetallicMap" },
			{ ShaderDataType::Int, "useUseRoughnessMap" },
			{ ShaderDataType::Int, "useUseAOMap" },

			{ ShaderDataType::Int, "AlbedroIndex" },
			{ ShaderDataType::Int, "NormalIndex" },
			{ ShaderDataType::Int, "MetallicIndex" },
			{ ShaderDataType::Int, "RoughnessIndex" },
			{ ShaderDataType::Int, "AOIndex" },

			{ ShaderDataType::Float, "Metallic" },
			{ ShaderDataType::Float, "Roughness" }
		};

		m_Instances.resize(2);
		m_Instances[0] = instanceOne;
		m_Instances[1] = instanceTwo;

		m_ModelViews.resize(2);
		CommandSystem::ComposeTransform(m_Pos, m_Rot, m_Scale, true, m_ModelViews[0]);
		CommandSystem::ComposeTransform(m_Pos + glm::vec3(2, 4, 4), m_Rot, m_Scale, true, m_ModelViews[1]);

		m_InstanceVB = VertexBuffer::Create(m_Instances.data(),
			static_cast<uint32_t>(sizeof(PBRVertexInstanced) * m_Instances.size()));


		//SSAO

		struct SSAOTempUbo
		{
			std::array<glm::vec4, 64> kernel;
		} ssaoTempData;

		SSAOGenerator::Generate(m_SSAONoise, ssaoTempData.kernel);

		VulkanPBR::Init("../Resources/Textures/gcanyon_cube.ktx", TextureFormat::R16G16B16A16_SFLOAT);

		// Editor Camera
		EditorCameraCreateInfo cameraCI = {};
		{
			cameraCI.IsFramebufferTargetsSwapchain = true;
			m_EditorCamera = std::make_shared<EditorCamera>(&cameraCI);
		}

		// MRT Framebufefr
		FramebufferSpecification MRTframebefferSpec = {};
		{
			MRTframebefferSpec.IsUseMRT = true;
			m_DeferredFrameBuffer = Framebuffer::Create(MRTframebefferSpec);
		}

		// SSAO Framebuffer
		FramebufferSpecification SSAOFramebuffer = {};
		{
			SSAOFramebuffer.Height = 2048;
			SSAOFramebuffer.Width = 2048;
			SSAOFramebuffer.IsUseMSAA = false;
			m_SSAOFrameBuffer = Framebuffer::Create(SSAOFramebuffer);
		}

		// SSAO Blur Framebuffer
		FramebufferSpecification SSAOBlurFramebuffer = {};
		{
			SSAOBlurFramebuffer.Height = 2048;
			SSAOBlurFramebuffer.Width = 2048;
			SSAOBlurFramebuffer.IsUseMSAA = false;
			m_SSAOBlurFrameBuffer = Framebuffer::Create(SSAOBlurFramebuffer);
		}

		// SkyBox Framebuffer
		FramebufferSpecification SkyBoxFramebuffer = {};
		{
			SkyBoxFramebuffer.Height = 2048;
			SkyBoxFramebuffer.Width = 2048;
			SkyBoxFramebuffer.IsUseMSAA = false;
			m_SkyboxFrameBuffer = Framebuffer::Create(SkyBoxFramebuffer);
		}

		// Model Textures
		{
			m_Tetxure1 = Texture::Create("../Resources/SMGtextureSet_Base_Color.png");
			m_Tetxure3 = Texture::Create("../Resources/SMGtextureSet_Normal_DirectX.png");
			m_Tetxure2 = Texture::Create("../Resources/SMGtextureSet_Metallic.png");
			m_Tetxure4 = Texture::Create("../Resources/SMGtextureSet_Roughness.png");
			m_Tetxure5 = Texture::Create("../Resources/SMGtextureSet_AO.png");
		}

		float quadVertices[] = {
			// positions   // texCoords
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f,
			-1.0f,  1.0f,  0.0f, 1.0f
		};

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

		struct FullSreenData
		{
			glm::vec2 pos;
			glm::vec2 uv;
		};

		BufferLayout FullSreenlayout =
		{
			{ ShaderDataType::Float2, "aPos" },
			{ ShaderDataType::Float2, "aUV" },
		};

		auto FullScreenVB = VertexBuffer::Create(quadVertices, sizeof(quadVertices));
		auto FullScreenID = IndexBuffer::Create(squareIndices, 6);

		// Model
		m_TestMesh = Mesh::Create("../Resources/30_SMG_LP.obj");

		// MRT
		{
			m_Pipeline = std::make_shared<GraphicsPipeline>();
			GraphicsPipelineShaderCreateInfo shaderCI = {};
			{
				shaderCI.FilePaths[ShaderType::Vertex] = "../Resources/Shaders/MRT_Vulkan_Vertex.glsl";
				shaderCI.FilePaths[ShaderType::Fragment] = "../Resources/Shaders/MRT_Vulkan_Fragment.glsl";
				shaderCI.StorageBuffersSizes[25] = { sizeof(glm::mat4) * 2 };
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
				DynamicPipelineCI.VertexInputInfos = { VertexInputInfo(sizeof(PBRVertex), mainLayout),
					VertexInputInfo(sizeof(PBRVertexInstanced), PBRInstanceLayout, true) };

				DynamicPipelineCI.PipelineName = "Deferred_Rendering";
				DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
				DynamicPipelineCI.IsUseMRT = true;
			}

			bool result = m_Pipeline->Create(&DynamicPipelineCI);
			assert(result == true);

			m_Pipeline->SubmitStorageBuffer(25, sizeof(glm::mat4) * 2, m_ModelViews.data());

			m_Pipeline->SetVertexBuffers({ m_TestMesh->GetVertexBuffer() });
			m_Pipeline->SetIndexBuffers({ m_TestMesh->GetIndexBuffer() });

			m_Pipeline->UpdateSamplers({ m_Tetxure1 }, 5); //albedo
			m_Pipeline->UpdateSamplers({ m_Tetxure3 }, 6); //normal
			m_Pipeline->UpdateSamplers({ m_Tetxure5 }, 7); //ao
			m_Pipeline->UpdateSamplers({ m_Tetxure2 }, 8); //metallic
			m_Pipeline->UpdateSamplers({ m_Tetxure4 }, 9); //roughness
		}

		// SSAO + SSAO Blur
		{
			// SSAO
			{
				m_SSAOPipeline = std::make_shared<GraphicsPipeline>();

				GraphicsPipelineShaderCreateInfo shaderCI = {};
				{
					shaderCI.FilePaths[ShaderType::Vertex] = "../Resources/Shaders/GenVertex_Vulkan_Vertex.glsl";
					shaderCI.FilePaths[ShaderType::Fragment] = "../Resources/Shaders/SSAO_Vulkan_Fragment.glsl";
				};

				GraphicsPipelineCreateInfo DynamicPipelineCI = {};
				{
					DynamicPipelineCI.VertexInputInfos = { VertexInputInfo(sizeof(FullSreenData), FullSreenlayout) };
					DynamicPipelineCI.PipelineName = "SSAO_Pipeline";
					DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
					DynamicPipelineCI.IsDepthTestEnabled = false;
					DynamicPipelineCI.IsUseMSAA = false;
				}

				bool result = m_SSAOPipeline->Create(&DynamicPipelineCI);
				assert(result == true);

				m_SSAOPipeline->SumbitUniformBuffer(25, sizeof(SSAOTempUbo), &ssaoTempData);
				m_SSAOPipeline->UpdateSampler(m_SSAONoise, 2);

#ifndef SMOLENGINE_OPENGL_IMPL
				auto& pass = m_DeferredFrameBuffer->GetVulkanFramebuffer().GetDeferredPass();

				m_SSAOPipeline->UpdateVulkanImageDescriptor(0, pass.positionImageInfo);
				m_SSAOPipeline->UpdateVulkanImageDescriptor(1, pass.normalsImageInfo);
#endif

				m_SSAOPipeline->SetVertexBuffers({ FullScreenVB });
				m_SSAOPipeline->SetIndexBuffers({ FullScreenID });
			}

			// Blur
			{
				m_SSAOBlurPipeline = std::make_shared<GraphicsPipeline>();

				GraphicsPipelineShaderCreateInfo shaderCI = {};
				{
					shaderCI.FilePaths[ShaderType::Vertex] = "../Resources/Shaders/GenVertex_Vulkan_Vertex.glsl";
					shaderCI.FilePaths[ShaderType::Fragment] = "../Resources/Shaders/SSAO_Blur_Vulkan_Fragment.glsl";
				};

				GraphicsPipelineCreateInfo DynamicPipelineCI = {};
				{
					DynamicPipelineCI.VertexInputInfos = { VertexInputInfo(sizeof(FullSreenData), FullSreenlayout) };
					DynamicPipelineCI.PipelineName = "SSAO_Blur_Pipeline";
					DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
					DynamicPipelineCI.IsDepthTestEnabled = false;
					DynamicPipelineCI.IsUseMSAA = false;
				}

				bool result = m_SSAOBlurPipeline->Create(&DynamicPipelineCI);
				assert(result == true);

#ifndef SMOLENGINE_OPENGL_IMPL
				auto& imageInfo = m_SSAOFrameBuffer->GetVulkanFramebuffer().GetOffscreenPass().colorImageInfo;
				m_SSAOBlurPipeline->UpdateVulkanImageDescriptor(0, imageInfo);
#endif
			}

			m_SSAOBlurPipeline->SetVertexBuffers({ FullScreenVB });
			m_SSAOBlurPipeline->SetIndexBuffers({ FullScreenID });

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
				shaderCI.FilePaths[ShaderType::Vertex] = "../Resources/Shaders/GenVertex_Vulkan_Vertex.glsl";
				shaderCI.FilePaths[ShaderType::Fragment] = "../Resources/Shaders/PBR_MRT_Vulkan_Fragment.glsl";
			};

			GraphicsPipelineCreateInfo DynamicPipelineCI = {};
			{
				DynamicPipelineCI.VertexInputInfos = { VertexInputInfo(sizeof(FullSreenData), FullSreenlayout) };
				DynamicPipelineCI.PipelineName = "Deferred_Rendering_Combination";
				DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
				DynamicPipelineCI.IsTargetsSwapchain = true;
			}

			bool result = m_CombinationPipeline->Create(&DynamicPipelineCI);
			assert(result == true);

			m_CombinationPipeline->SetVertexBuffers({ FullScreenVB });
			m_CombinationPipeline->SetIndexBuffers({ FullScreenID });

#ifndef SMOLENGINE_OPENGL_IMPL

			auto& pass = m_DeferredFrameBuffer->GetVulkanFramebuffer().GetDeferredPass();

			m_CombinationPipeline->UpdateVulkanImageDescriptor(0, m_SkyboxFrameBuffer->GetVulkanFramebuffer().GetOffscreenPass().colorImageInfo);
			m_CombinationPipeline->UpdateVulkanImageDescriptor(1, pass.positionImageInfo);
			m_CombinationPipeline->UpdateVulkanImageDescriptor(2, pass.normalsImageInfo);
			m_CombinationPipeline->UpdateVulkanImageDescriptor(3, pass.colorImageInfo);
			m_CombinationPipeline->UpdateVulkanImageDescriptor(4, pass.pbrImageInfo);
			m_CombinationPipeline->UpdateVulkanImageDescriptor(5, m_SSAOFrameBuffer->GetVulkanFramebuffer().GetOffscreenPass().colorImageInfo);
			m_CombinationPipeline->UpdateVulkanImageDescriptor(6, m_SSAOBlurFrameBuffer->GetVulkanFramebuffer().GetOffscreenPass().colorImageInfo);


			m_CombinationPipeline->UpdateVulkanImageDescriptor(7, VulkanPBR::GetIrradianceImageInfo());
			m_CombinationPipeline->UpdateVulkanImageDescriptor(8, VulkanPBR::GetBRDFLUTImageInfo());
			m_CombinationPipeline->UpdateVulkanImageDescriptor(9, VulkanPBR::GetPrefilteredCubeImageInfo());
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
		ImGui::Checkbox("SSAO Enabled", &m_SSAOEnabled);

		ImGui::NewLine();
		std::vector<const char*> charitems = { "Final composition", "Position", "Normals", "Albedo", "Ambient Occlusion", "Metallic", "Roughness" };
		uint32_t itemCount = static_cast<uint32_t>(charitems.size());
		bool res = ImGui::Combo("Display", &m_Params.mode, &charitems[0], itemCount, itemCount);

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
						glm::mat4 view;

						float nearPlane;
						float farPlane;

					} pc;

					pc.proj = m_EditorCamera->GetProjection();
					pc.view = m_EditorCamera->GetViewMatrix();
					pc.nearPlane = m_EditorCamera->GetNearClip();
					pc.farPlane = m_EditorCamera->GetFarClip();

					m_Pipeline->SumbitPushConstant(ShaderType::Vertex, sizeof(PushConsant), &pc);
					m_Pipeline->DrawInstanced(m_TestMesh, { m_InstanceVB }, 2);
				}
				m_Pipeline->EndRenderPass();
			}

			// Skybox
			{
#ifndef SMOLENGINE_OPENGL_IMPL
				m_SkyboxPipeline->SetCommandBuffer(cmdBuffer);
#endif
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
			}

			if (m_SSAOEnabled)
			{
				// SSAO
				{
#ifndef SMOLENGINE_OPENGL_IMPL
					m_SSAOPipeline->SetCommandBuffer(cmdBuffer);
#endif
					m_SSAOPipeline->BeginRenderPass(m_SSAOFrameBuffer);
					{
						m_SSAOPipeline->ClearColors();

						struct PushConstant
						{
							glm::mat4 proj;

						} pc;

						pc.proj = m_EditorCamera->GetProjection();

						m_SSAOPipeline->SumbitPushConstant(ShaderType::Fragment, sizeof(PushConstant), &pc);
						m_SSAOPipeline->DrawIndexed();
					}
					m_SSAOPipeline->EndRenderPass();
				}

				// SSAO Blur
				{
#ifndef SMOLENGINE_OPENGL_IMPL
					m_SSAOBlurPipeline->SetCommandBuffer(cmdBuffer);
#endif
					m_SSAOBlurPipeline->BeginRenderPass(m_SSAOBlurFrameBuffer);
					{
						m_SSAOBlurPipeline->ClearColors();
						m_SSAOBlurPipeline->DrawIndexed();
					}
					m_SSAOBlurPipeline->EndRenderPass();
				}
			}

			m_Pipeline->EndCommandBuffer(); // Submit work and wait 
		}

		// Final + Lighting
		{
			m_CombinationPipeline->BeginCommandBuffer(true);
			m_CombinationPipeline->BeginBufferSubmit();

			m_Params.viewPos = glm::vec4(m_EditorCamera->GetPosition(), 0);

			m_SSAOEnabled ? m_Params.ssaoEnabled = 1 : m_Params.ssaoEnabled = 0;

			m_CombinationPipeline->SumbitUniformBuffer(15, sizeof(UBOMRTParams), &m_Params);

			m_CombinationPipeline->BeginRenderPass(m_EditorCamera->GetFramebuffer());
			{
				m_CombinationPipeline->ClearColors();
				m_CombinationPipeline->DrawIndexed();
			}
			m_CombinationPipeline->EndRenderPass();
			m_CombinationPipeline->EndBufferSubmit();
			m_CombinationPipeline->EndCommandBuffer();
		}
		
	}
}