#include "stdafx.h"
#include "DeferredRenderingTestLayer.h"
#include "Core/Application.h"
#include "Core/Window.h"
#include "Core/FilePaths.h"

#include "Renderer/Shader.h"
#include "Renderer/CubeTexture.h"
#include "Renderer/Mesh.h"

#include "Renderer/Vulkan/Vulkan.h"
#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/SSAOGenerator.h"
#include "Renderer/MaterialLibrary.h"
#include "Renderer/Vulkan/VulkanPBR.h"

#include "ECS/Systems/CommandSystem.h"

namespace SmolEngine
{
	void DeferredRenderingTest::OnAttach()
	{
#ifdef SMOLENGINE_OPENL_IMPL // Vulkan support only
		return;
#endif
		// Create materials

		MaterialCreateInfo materialCI = {};
		materialCI.Name = "Brick";
		assert(MaterialLibrary::GetSinglenton()->Add(&materialCI) > -1);
		auto material_ = MaterialLibrary::GetSinglenton()->GetMaterial(0);


		MaterialCreateInfo chairMaterialCI = {};
		materialCI.Name = "Wooden Chair";
		int32_t id = MaterialLibrary::GetSinglenton()->Add(&materialCI);
		auto material = MaterialLibrary::GetSinglenton()->GetMaterial(id);
		// Models buffer
		m_ModelViews.resize(1);
		CommandSystem::ComposeTransform(m_Pos, m_Rot, m_Scale, true, m_ModelViews[0]);

		//SSAO

		struct SSAOTempUbo
		{
			std::array<glm::vec4, 32> kernel;
		} ssaoTempData;

		SSAOGenerator::Generate(m_SSAONoise, ssaoTempData.kernel);

		VulkanPBR::Init("../Resources/Textures/gcanyon_cube.ktx", TextureFormat::R16G16B16A16_SFLOAT);

		// Editor Camera
		EditorCameraCreateInfo cameraCI = {};
		{
			m_EditorCamera = std::make_shared<EditorCamera>(&cameraCI);
		}

		// Framebuffer Editor
		FramebufferSpecification framebufferEditor;
		{
			framebufferEditor.Width = Application::GetApplication().GetWindowWidth();
			framebufferEditor.Height = Application::GetApplication().GetWindowHeight();
			framebufferEditor.bUseMSAA = true;
			framebufferEditor.bTargetsSwapchain = true;
			framebufferEditor.Attachments = { FramebufferAttachment(AttachmentFormat::Color, true) };

			auto frameBuffer = Framebuffer::Create(framebufferEditor);
			m_EditorCamera->SetFramebuffers({ frameBuffer });
		}

		// MRT Framebufefr
		FramebufferSpecification MRTframebefferSpec = {};
		{
			MRTframebefferSpec.bUseMSAA = false;
			MRTframebefferSpec.bTargetsSwapchain = false;
			MRTframebefferSpec.Height = 2048;
			MRTframebefferSpec.Width = 2048;

			MRTframebefferSpec.Attachments.resize(4);
			MRTframebefferSpec.Attachments[0] = FramebufferAttachment(AttachmentFormat::SFloat4_32, true, "Position");
			MRTframebefferSpec.Attachments[1] = FramebufferAttachment(AttachmentFormat::SFloat4_32, true, "Normals");
			MRTframebefferSpec.Attachments[2] = FramebufferAttachment(AttachmentFormat::SFloat4_32, true, "Pbr");
			MRTframebefferSpec.Attachments[3] = FramebufferAttachment(AttachmentFormat::SFloat4_32,  true, "Albedro");

			m_DeferredFrameBuffer = Framebuffer::Create(MRTframebefferSpec);
		}

		// SSAO Framebuffer
		FramebufferSpecification SSAOFramebuffer = {};
		{
			SSAOFramebuffer.Width = Application::GetApplication().GetWindowWidth();
			SSAOFramebuffer.Height = Application::GetApplication().GetWindowHeight();
			SSAOFramebuffer.bUseMSAA = false;
			SSAOFramebuffer.bTargetsSwapchain = false;
			SSAOFramebuffer.Attachments = { FramebufferAttachment(AttachmentFormat::UNORM_8, true) };

			m_SSAOFrameBuffer = Framebuffer::Create(SSAOFramebuffer);
		}

		// SSAO Blur Framebuffer
		FramebufferSpecification SSAOBlurFramebuffer = {};
		{
			SSAOBlurFramebuffer.Width = Application::GetApplication().GetWindowWidth();
			SSAOBlurFramebuffer.Height = Application::GetApplication().GetWindowHeight();
			SSAOBlurFramebuffer.bUseMSAA = false;
			SSAOBlurFramebuffer.bTargetsSwapchain = false;
			SSAOBlurFramebuffer.Attachments = { FramebufferAttachment(AttachmentFormat::UNORM_8, true) };

			m_SSAOBlurFrameBuffer = Framebuffer::Create(SSAOBlurFramebuffer);
		}

		// SkyBox Framebuffer
		FramebufferSpecification SkyBoxFramebuffer = {};
		{
			SkyBoxFramebuffer.Height = 2048;
			SkyBoxFramebuffer.Width = 2048;
			SkyBoxFramebuffer.bUseMSAA = false;
			SkyBoxFramebuffer.bTargetsSwapchain = false;
			SkyBoxFramebuffer.Attachments = { FramebufferAttachment(AttachmentFormat::Color, true) };

			m_SkyboxFrameBuffer = Framebuffer::Create(SkyBoxFramebuffer);
		}

		// Load Textures
		{
			m_Tetxure1 = Texture::Create(Resources + "WoodenChair_01_16-bit_Diffuse.png");
			m_Tetxure3 = Texture::Create(Resources + "WoodenChair_01_16-bit_Normal.png");
			m_Tetxure2 = Texture::Create(Resources + "WoodenChair_01_16-bit_Metallic.png");
			m_Tetxure4 = Texture::Create(Resources + "WoodenChair_01_16-bit_Roughness.png");
			m_Tetxure5 = Texture::Create(Resources + "WoodenChair_01_16-bit_Height.png");

			m_BrickAlbedro = Texture::Create(Resources + "bricks_diffuse.png");
			m_BrickNormal = Texture::Create(Resources + "bricks_normal.png");
			m_BrickRoughness = Texture::Create(Resources + "bricks_roughness.png");
		}

		float quadVertices[] = {
			// positions   // texCoords
			-1.0f, -1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 1.0f,
			 1.0f,  1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f,  0.0f, 0.0f
		};

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

		struct FullSreenData
		{
			glm::vec2 pos;
			glm::vec2 uv;
		};

		BufferLayout FullSreenlayout =
		{
			{ DataTypes::Float2, "aPos" },
			{ DataTypes::Float2, "aUV" },
		};

		auto FullScreenVB = VertexBuffer::Create(quadVertices, sizeof(quadVertices));
		auto FullScreenID = IndexBuffer::Create(squareIndices, 6);

		// Model
		m_TestMesh = Mesh::Create(Resources + "WoodenChair_01.FBX");
		m_SponzaMesh = Mesh::Create(Resources + "sponza.glb");
		m_PlaneMesh = Mesh::Create(Resources + "plane.glb");

		// Default PBR
		{
			m_PBRPipeline = std::make_shared<GraphicsPipeline>();
			GraphicsPipelineShaderCreateInfo shaderCI = {};
			{
				shaderCI.FilePaths[ShaderType::Vertex] = Resources + "Shaders/PBR_Vulkan_Vertex.glsl";
				shaderCI.FilePaths[ShaderType::Fragment] = Resources + "Shaders/PBR_Vulkan_Fragment.glsl";
			};

			BufferLayout mainLayout =
			{
				{ DataTypes::Float3, "aPos" },
				{ DataTypes::Float3, "aNormal" },
				{ DataTypes::Float4, "aTangent" },
				{ DataTypes::Float2, "aUV" },
				{ DataTypes::Float4, "aColor" }
			};


			GraphicsPipelineCreateInfo DynamicPipelineCI = {};
			{
				DynamicPipelineCI.VertexInputInfos = { VertexInputInfo(sizeof(PBRVertex), mainLayout) };

				DynamicPipelineCI.PipelineName = "PBR_Rendering";
				DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
				DynamicPipelineCI.TargetFramebuffer = m_EditorCamera->GetFramebuffer();
			}

			auto result = m_PBRPipeline->Create(&DynamicPipelineCI);
			assert(result == PipelineCreateResult::SUCCESS);

			m_PBRPipeline->UpdateSampler(m_Tetxure1, 5); //albedo
			m_PBRPipeline->UpdateSampler(m_Tetxure3, 6); //normal
			m_PBRPipeline->UpdateSampler(m_Tetxure5, 7); //ao
			m_PBRPipeline->UpdateSampler( m_Tetxure2, 8); //metallic
			m_PBRPipeline->UpdateSampler(m_Tetxure4, 9); //roughness

			m_PBRPipeline->SubmitUniformBuffer(12, sizeof(PBRParams), &m_PBRParams);

#ifndef SMOLENGINE_OPENGL_IMPL
			m_PBRPipeline->UpdateVulkanImageDescriptor(2, VulkanPBR::GetIrradianceImageInfo());
			m_PBRPipeline->UpdateVulkanImageDescriptor(3, VulkanPBR::GetBRDFLUTImageInfo());
			m_PBRPipeline->UpdateVulkanImageDescriptor(4, VulkanPBR::GetPrefilteredCubeImageInfo());
#endif
		}

		// MRT
		{
			m_Pipeline = std::make_shared<GraphicsPipeline>();
			GraphicsPipelineShaderCreateInfo shaderCI = {};
			{
				shaderCI.FilePaths[ShaderType::Vertex] = "../Resources/Shaders/MRT_Vulkan_Vertex.glsl";
				shaderCI.FilePaths[ShaderType::Fragment] = "../Resources/Shaders/MRT_Vulkan_Fragment.glsl";

				shaderCI.StorageBuffersSizes[25] = { sizeof(glm::mat4) * m_ModelViews.size() };
				shaderCI.StorageBuffersSizes[26] = { sizeof(Material) * 1000 };
			};

			BufferLayout mainLayout =
			{
				{ DataTypes::Float3, "aPos" },
				{ DataTypes::Float3, "aNormal" },
				{ DataTypes::Float4, "aTangent" },
				{ DataTypes::Float2, "aUV" },
				{ DataTypes::Float4, "aColor" }
			};

			GraphicsPipelineCreateInfo DynamicPipelineCI = {};
			{
				DynamicPipelineCI.VertexInputInfos = { VertexInputInfo(sizeof(PBRVertex), mainLayout) };

				DynamicPipelineCI.PipelineName = "Deferred_Rendering";
				DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
				DynamicPipelineCI.TargetFramebuffer = m_DeferredFrameBuffer;
			}

			auto result = m_Pipeline->Create(&DynamicPipelineCI);
			assert(result == PipelineCreateResult::SUCCESS);

			m_Pipeline->SubmitStorageBuffer(25, sizeof(glm::mat4) * m_ModelViews.size(), m_ModelViews.data());

			m_Pipeline->UpdateSamplers({ m_Tetxure1, m_BrickAlbedro }, 5); //albedo
			m_Pipeline->UpdateSamplers({ m_Tetxure3, m_BrickNormal }, 6); //normal
			m_Pipeline->UpdateSamplers({ m_Tetxure5 }, 7); //ao
			m_Pipeline->UpdateSamplers({ m_Tetxure2 }, 8); //metallic
			m_Pipeline->UpdateSamplers({ m_Tetxure4, m_BrickRoughness }, 9); //roughness
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
					DynamicPipelineCI.TargetFramebuffer = m_SSAOFrameBuffer;
				}

				auto result = m_SSAOPipeline->Create(&DynamicPipelineCI);
				assert(result == PipelineCreateResult::SUCCESS);

				m_SSAOPipeline->SubmitUniformBuffer(25, sizeof(SSAOTempUbo), &ssaoTempData);
				m_SSAOPipeline->UpdateSampler(m_SSAONoise, 2);

#ifndef SMOLENGINE_OPENGL_IMPL
				auto& pass = m_DeferredFrameBuffer->GetVulkanFramebuffer();

				m_SSAOPipeline->UpdateVulkanImageDescriptor(0, pass.GetAttachment(std::string("Position"))->ImageInfo);
				m_SSAOPipeline->UpdateVulkanImageDescriptor(1, pass.GetAttachment(std::string("Normals"))->ImageInfo);
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
					DynamicPipelineCI.TargetFramebuffer = m_SSAOBlurFrameBuffer;
				}

				auto result = m_SSAOBlurPipeline->Create(&DynamicPipelineCI);
				assert(result == PipelineCreateResult::SUCCESS);

#ifndef SMOLENGINE_OPENGL_IMPL
				m_SSAOBlurPipeline->UpdateVulkanImageDescriptor(0, m_SSAOFrameBuffer->GetVulkanFramebuffer().GetAttachment()->ImageInfo);
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
				{ DataTypes::Float3, "aPos" }
			};

			GraphicsPipelineCreateInfo DynamicPipelineCI = {};
			{
				DynamicPipelineCI.VertexInputInfos = { VertexInputInfo(sizeof(SkyBoxData), layout) };
				DynamicPipelineCI.PipelineName = "Skybox_Test";
				DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
				DynamicPipelineCI.bDepthTestEnabled = false;
				DynamicPipelineCI.TargetFramebuffer = m_SkyboxFrameBuffer;
			}

			auto result = m_SkyboxPipeline->Create(&DynamicPipelineCI);
			assert(result == PipelineCreateResult::SUCCESS);

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
				DynamicPipelineCI.TargetFramebuffer = m_EditorCamera->GetFramebuffer();
			}

			auto result = m_CombinationPipeline->Create(&DynamicPipelineCI);
			assert(result == PipelineCreateResult::SUCCESS);

			m_CombinationPipeline->SubmitUniformBuffer(16, sizeof(SkyLights), &m_SkyLights);
			m_CombinationPipeline->SubmitUniformBuffer(17, sizeof(PointLights), &m_PointLights);

			m_CombinationPipeline->SetVertexBuffers({ FullScreenVB });
			m_CombinationPipeline->SetIndexBuffers({ FullScreenID });

#ifndef SMOLENGINE_OPENGL_IMPL

			auto& pass = m_DeferredFrameBuffer->GetVulkanFramebuffer();

			m_CombinationPipeline->UpdateVulkanImageDescriptor(0, m_SkyboxFrameBuffer->GetVulkanFramebuffer().GetAttachment()->ImageInfo);
			m_CombinationPipeline->UpdateVulkanImageDescriptor(1, pass.GetAttachment(std::string("Position"))->ImageInfo);
			m_CombinationPipeline->UpdateVulkanImageDescriptor(2, pass.GetAttachment(std::string("Normals"))->ImageInfo); 
			m_CombinationPipeline->UpdateVulkanImageDescriptor(4, pass.GetAttachment(std::string("Pbr"))->ImageInfo);
			m_CombinationPipeline->UpdateVulkanImageDescriptor(3, pass.GetAttachment(std::string("Albedro"))->ImageInfo); 
			m_CombinationPipeline->UpdateVulkanImageDescriptor(5, m_SSAOFrameBuffer->GetVulkanFramebuffer().GetAttachment()->ImageInfo);
			m_CombinationPipeline->UpdateVulkanImageDescriptor(6, m_SSAOBlurFrameBuffer->GetVulkanFramebuffer().GetAttachment()->ImageInfo);


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

		if (ImGui::Button("Reload MRT Shader"))
		{
			if (m_Pipeline->Reload() == PipelineCreateResult::SUCCESS)
			{
				m_Pipeline->SubmitStorageBuffer(25, sizeof(glm::mat4) * m_ModelViews.size(), m_ModelViews.data());

				m_Pipeline->UpdateSamplers({ m_Tetxure1, m_BrickAlbedro }, 5); //albedo
				m_Pipeline->UpdateSamplers({ m_Tetxure3, m_BrickNormal }, 6); //normal
				m_Pipeline->UpdateSamplers({ m_Tetxure5 }, 7); //ao
				m_Pipeline->UpdateSamplers({ m_Tetxure2 }, 8); //metallic
				m_Pipeline->UpdateSamplers({ m_Tetxure4, m_BrickRoughness }, 9); //roughness

				auto& pass = m_DeferredFrameBuffer->GetVulkanFramebuffer();
				m_SSAOPipeline->UpdateVulkanImageDescriptor(0, pass.GetAttachment(std::string("Position"))->ImageInfo);
				m_SSAOPipeline->UpdateVulkanImageDescriptor(1, pass.GetAttachment(std::string("Normals"))->ImageInfo);
				m_CombinationPipeline->UpdateVulkanImageDescriptor(1, pass.GetAttachment(std::string("Position"))->ImageInfo);
				m_CombinationPipeline->UpdateVulkanImageDescriptor(2, pass.GetAttachment(std::string("Normals"))->ImageInfo);
				m_CombinationPipeline->UpdateVulkanImageDescriptor(4, pass.GetAttachment(std::string("Pbr"))->ImageInfo);
				m_CombinationPipeline->UpdateVulkanImageDescriptor(3, pass.GetAttachment(std::string("Albedro"))->ImageInfo);
			}
		}

		if (ImGui::Button("Reload Combination Shader"))
		{
			if (m_CombinationPipeline->Reload() == PipelineCreateResult::SUCCESS)
			{
				auto& pass = m_DeferredFrameBuffer->GetVulkanFramebuffer();

				m_CombinationPipeline->UpdateVulkanImageDescriptor(0, m_SkyboxFrameBuffer->GetVulkanFramebuffer().GetAttachment()->ImageInfo);
				m_CombinationPipeline->UpdateVulkanImageDescriptor(1, pass.GetAttachment(std::string("Position"))->ImageInfo);
				m_CombinationPipeline->UpdateVulkanImageDescriptor(2, pass.GetAttachment(std::string("Normals"))->ImageInfo);
				m_CombinationPipeline->UpdateVulkanImageDescriptor(4, pass.GetAttachment(std::string("Pbr"))->ImageInfo);
				m_CombinationPipeline->UpdateVulkanImageDescriptor(3, pass.GetAttachment(std::string("Albedro"))->ImageInfo);
				m_CombinationPipeline->UpdateVulkanImageDescriptor(5, m_SSAOFrameBuffer->GetVulkanFramebuffer().GetAttachment()->ImageInfo);
				m_CombinationPipeline->UpdateVulkanImageDescriptor(6, m_SSAOBlurFrameBuffer->GetVulkanFramebuffer().GetAttachment()->ImageInfo);


				m_CombinationPipeline->UpdateVulkanImageDescriptor(7, VulkanPBR::GetIrradianceImageInfo());
				m_CombinationPipeline->UpdateVulkanImageDescriptor(8, VulkanPBR::GetBRDFLUTImageInfo());
				m_CombinationPipeline->UpdateVulkanImageDescriptor(9, VulkanPBR::GetPrefilteredCubeImageInfo());
			}
		}

		ImGui::Checkbox("SSAO Enabled", &m_SSAOEnabled);

		ImGui::NewLine();
		{
			std::vector<const char*> charitems = { "Final composition", "Position", "Normals", "Albedo", "Ambient Occlusion", "Metallic", "Roughness" };
			uint32_t itemCount = static_cast<uint32_t>(charitems.size());
			bool res = ImGui::Combo("Display", &m_DisplayMode, &charitems[0], itemCount, itemCount);
		}

		ImGui::NewLine();
		{
			std::vector<const char*> MaterialsItems = { "Default", "Chair" };
			uint32_t itemCount = static_cast<uint32_t>(MaterialsItems.size());
			bool res = ImGui::Combo("Material", &m_MaterialIndex, &MaterialsItems[0], itemCount, itemCount);
		}

		ImGui::End();
	}

	void DeferredRenderingTest::OnUpdate(DeltaTime deltaTime)
	{
		m_EditorCamera->OnUpdate(deltaTime);

		BuildTestCommandBuffer();
	}

	void DeferredRenderingTest::BuildTestCommandBuffer()
	{
		static bool defPBR = false;
		if (defPBR)
		{
			// Default PBR Test
			{
				m_PBRPipeline->BeginCommandBuffer(true);
				m_PBRPipeline->BeginRenderPass();
				{
					struct PushConsant
					{
						glm::mat4 proj;
						glm::mat4 model;
						glm::mat4 view;

						glm::vec3 camPos;
					} pc;

					pc.proj = m_EditorCamera->GetProjection();
					pc.model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					pc.view = m_EditorCamera->GetViewMatrix();
					pc.camPos = m_EditorCamera->GetPosition();

					m_PBRPipeline->SubmitPushConstant(ShaderType::Vertex, sizeof(PushConsant), &pc);
					m_PBRPipeline->DrawMesh(m_SponzaMesh.get());
				}
				m_PBRPipeline->EndRenderPass();
			}
			return;
		}

		bool useMainCmdBuffer = true;
		VkCommandBuffer cmdBuffer;
		// Offscreen MRT + SkyBox
		{

			// MRT
			{
				// Update materials
				void* data = nullptr;
				uint32_t size = 0;
				MaterialLibrary::GetSinglenton()->GetMaterialsPtr(data, size);
				m_Pipeline->SubmitStorageBuffer(26, size, data);

				m_Pipeline->BeginCommandBuffer(useMainCmdBuffer);
				m_Pipeline->BeginRenderPass();
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

						int modelIndex;
						int materialIndex;

					} pc;

					pc.proj = m_EditorCamera->GetProjection();
					pc.view = m_EditorCamera->GetViewMatrix();
					pc.nearPlane = m_EditorCamera->GetNearClip();
					pc.farPlane = m_EditorCamera->GetFarClip();

					pc.modelIndex = 0;
					pc.materialIndex = m_MaterialIndex;

					m_Pipeline->SubmitPushConstant(ShaderType::Vertex, sizeof(PushConsant), &pc);
					m_Pipeline->DrawMesh(m_PlaneMesh.get());
				}
				m_Pipeline->EndRenderPass();
			}

			// Skybox
			{
#ifndef SMOLENGINE_OPENGL_IMPL
				m_SkyboxPipeline->SetCommandBuffer(cmdBuffer);
#endif
				m_SkyboxPipeline->BeginRenderPass();
				{
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

					m_SkyboxPipeline->SubmitPushConstant(ShaderType::Vertex, sizeof(PushConstant), &pc);
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
					m_SSAOPipeline->BeginRenderPass();
					{
						struct PushConstant
						{
							glm::mat4 proj;
							glm::mat4 view;

						} pc;

						pc.proj = m_EditorCamera->GetProjection();
						pc.view = m_EditorCamera->GetViewMatrix();

						m_SSAOPipeline->SubmitPushConstant(ShaderType::Fragment, sizeof(PushConstant), &pc);
						m_SSAOPipeline->DrawIndexed();
					}
					m_SSAOPipeline->EndRenderPass();
				}

				// SSAO Blur
				{
#ifndef SMOLENGINE_OPENGL_IMPL
					m_SSAOBlurPipeline->SetCommandBuffer(cmdBuffer);
#endif
					m_SSAOBlurPipeline->BeginRenderPass();
					{
						m_SSAOBlurPipeline->DrawIndexed();
					}
					m_SSAOBlurPipeline->EndRenderPass();
				}
			}
		}

		// Final + Lighting
		{
#ifndef SMOLENGINE_OPENGL_IMPL
		m_CombinationPipeline->SetCommandBuffer(cmdBuffer);
#endif
			m_CombinationPipeline->BeginRenderPass();
			{
				struct  PushConstant
				{
					glm::vec4 viewPos;
					glm::mat4 view;
					int displayMode;
					int ssaoEnabled;
				}pc;

				pc.viewPos = glm::vec4(m_EditorCamera->GetPosition(),  1) ;
				pc.view = m_EditorCamera->GetViewMatrix();
				pc.displayMode = m_DisplayMode;
				pc.ssaoEnabled = m_SSAOEnabled ? 1 : 0;

				m_CombinationPipeline->SubmitPushConstant(ShaderType::Fragment, sizeof(PushConstant), &pc);
				m_CombinationPipeline->DrawIndexed();
			}
			m_CombinationPipeline->EndRenderPass();
		}
	}
}