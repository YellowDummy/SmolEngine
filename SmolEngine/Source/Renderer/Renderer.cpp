#include "stdafx.h"
#include "Renderer.h"

#include "Renderer/GraphicsPipeline.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Shader.h"
#include "Renderer/Shared.h"
#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"
#include "Renderer/MaterialLibrary.h"
#include "Renderer/RendererShared.h"

#ifdef SMOLENGINE_OPENGL_IMPL
#include "Renderer/OpenGL/OpenglShader.h"
#include "Renderer/OpenGL/OpenglRendererAPI.h"
#else
#include "Renderer/Vulkan/VulkanPBR.h"
#endif

#include "ECS/Systems/CommandSystem.h"

namespace SmolEngine
{
	static const size_t                  s_MaxInstances = 500;
	static const size_t                  s_MaxPackages = 1200;
	static const size_t                  s_MaxDirectionalLights = 1000;
	static const size_t                  s_InstanceDataMaxCount = s_MaxPackages * s_MaxInstances;

	struct CommandBuffer
	{
		uint32_t                         InstancesCount = 0;
		uint32_t                         Offset = 0;
		Mesh*                            Mesh = nullptr;
	};									 
										 
	struct InstancesData
	{									 
		glm::mat4                        ModelView = glm::mat4(0.0f);
		glm::vec4                        Params = glm::vec4(0);
	};									 
										 
	struct InstancePackage				 
	{									 
		struct Package					 
		{								 
			int32_t                      MaterialID = 0;
			glm::vec3                    WorldPos = glm::vec3(0.0f);
			glm::vec3                    Rotation = glm::vec3(0.0f);
			glm::vec3                    Scale = glm::vec3(0.0f);
		};								 
										 
		uint32_t                         CurrentIndex = 0;
		Package                          Data[s_MaxInstances];
	};

	struct RendererData
	{
		RendererData()
		{
			m_Packages.reserve(s_MaxPackages);
		}

		// Bindings
		const uint32_t                   m_TexturesBinding = 24;
		const uint32_t                   m_ShaderDataBinding = 25;
		const uint32_t                   m_MaterialsBinding = 26;
		const uint32_t                   m_SceneDataBinding = 27;

		// States
		bool                             m_IsInitialized = false;

		// Pipelines
		Scope<GraphicsPipeline>          m_MainPipeline = nullptr;
		Scope<GraphicsPipeline>          m_SkyboxPipeline = nullptr;

		// Framebuffers
		Ref<Framebuffer>                 m_Framebuffer = nullptr;

		// Instance Data
		uint32_t                         m_Objects = 0;
		uint32_t                         m_InstanceDataIndex = 0;
		uint32_t                         m_UsedMeshesIndex = 0;
		uint32_t                         m_DrawListIndex = 0;
		uint32_t                         m_DirectionalLightIndex = 0;
		uint32_t                         m_MaxObjects = s_MaxPackages;

		// Buffers
		Mesh*                            m_UsedMeshes[s_MaxPackages];
		InstancesData                    m_InstancesData[s_InstanceDataMaxCount];
		CommandBuffer                    m_DrawList[s_MaxPackages];
		DirectionalLightBuffer           m_DirectionalLights[s_MaxDirectionalLights];

		std::unordered_map<Mesh*,
			InstancePackage>             m_Packages;

		// UBO's & Push Constants
		struct SceneData
		{
			glm::mat4                    Projection = glm::mat4(1.0f);
			glm::mat4                    View = glm::mat4(1.0f);
			glm::mat4                    SkyBoxMatrix = glm::mat4(1.0f);
			glm::vec4                    CamPos = glm::vec4(1.0f);

			glm::vec4                    Params = glm::vec4(2.5f, 4.0f, 1.0f, 1.0f);
		};

		struct PushConstant
		{
			int                          DataOffset = 0;
			int                          DirectionalLights = 0;
		};

		SceneData                        m_SceneData = {};
		PushConstant                     m_MainPushConstant = {};

		const size_t                     m_SceneDataSize = sizeof(SceneData);
		const size_t                     m_PushConstantSize = sizeof(PushConstant);
	};

	static RendererData* s_Data = nullptr;

	void Renderer::Init()
	{
		s_Data = new RendererData();
		InitPBR();
		InitFramebuffers();
		InitPipelines();

		s_Data->m_IsInitialized = true;
	}

	void Renderer::Shutdown()
	{
		delete s_Data;
	}

	void Renderer::BeginScene(const glm::mat4& proj, const glm::mat4& view, const glm::vec3& camPos)
	{
		s_Data->m_SceneData.View = view;
		s_Data->m_SceneData.Projection = proj;
		s_Data->m_SceneData.CamPos = glm::vec4(camPos, 1);
		s_Data->m_SceneData.SkyBoxMatrix = glm::mat4(glm::mat3(view));

		s_Data->m_MainPipeline->BeginCommandBuffer(true);
#if 0
		s_Data->m_MainPipeline->BeginRenderPass();
		{
			s_Data->m_MainPipeline->ClearColors();
		}
		s_Data->m_MainPipeline->EndRenderPass();
#endif

		StartNewBacth();
	}

	void Renderer::EndScene()
	{
		Flush();
		s_Data->m_MainPipeline->EndCommandBuffer();
	}

	void Renderer::Flush()
	{
		for (uint32_t i = 0; i < s_Data->m_UsedMeshesIndex; ++i)
		{
			// Getting values
			auto& cmd = s_Data->m_DrawList[s_Data->m_DrawListIndex];
			Mesh* mesh = s_Data->m_UsedMeshes[i];
			auto& instance = s_Data->m_Packages[mesh];

			// Setting draw list command
			cmd.Offset = s_Data->m_InstanceDataIndex;
			cmd.Mesh = mesh;
			cmd.InstancesCount = instance.CurrentIndex;

			for (uint32_t x = 0; x < instance.CurrentIndex; ++x)
			{
				auto& package = instance.Data[x];
				auto& shaderData = s_Data->m_InstancesData[s_Data->m_InstanceDataIndex];

				shaderData.Params.x = package.MaterialID;
				CommandSystem::ComposeTransform(package.WorldPos, package.Rotation, package.Scale, true, shaderData.ModelView);
				s_Data->m_InstanceDataIndex++;
			}

			instance.CurrentIndex = 0;
			s_Data->m_DrawListIndex++;
		}

		// Updates UBOs and SSBOs 
		s_Data->m_MainPipeline->BeginBufferSubmit();
		{
			// Updates scene data
			s_Data->m_MainPipeline->SubmitUniformBuffer(s_Data->m_SceneDataBinding, s_Data->m_SceneDataSize, &s_Data->m_SceneData);
			// FIX:
			s_Data->m_SkyboxPipeline->SubmitUniformBuffer(s_Data->m_SceneDataBinding, s_Data->m_SceneDataSize, &s_Data->m_SceneData);

			// Updates Directional Lights
			s_Data->m_MainPipeline->SubmitStorageBuffer(28, sizeof(DirectionalLightBuffer) * s_Data->m_DirectionalLightIndex, &s_Data->m_DirectionalLights);

			// Updates model views and material indexes
			s_Data->m_MainPipeline->SubmitStorageBuffer(s_Data->m_ShaderDataBinding, sizeof(InstancesData) * s_Data->m_InstanceDataIndex, &s_Data->m_InstancesData);

#ifdef SMOLENGINE_EDITOR
			// Update materials
			void* data = nullptr;
			uint32_t size = 0;
			MaterialLibrary::GetSinglenton()->GetMaterialsPtr(data, size);
			s_Data->m_MainPipeline->SubmitStorageBuffer(s_Data->m_MaterialsBinding, size, data);
#endif
		}

		// SkyBox
		s_Data->m_SkyboxPipeline->BeginCommandBuffer(true);
		s_Data->m_SkyboxPipeline->BeginRenderPass();
		{
			s_Data->m_SkyboxPipeline->ClearColors();
			s_Data->m_SkyboxPipeline->Draw(36);
		}
		s_Data->m_SkyboxPipeline->EndRenderPass();

		// Main
		s_Data->m_MainPipeline->BeginRenderPass();
		{
			for (uint32_t i = 0; i < s_Data->m_DrawListIndex; ++i)
			{
				auto& cmd = s_Data->m_DrawList[i];

				s_Data->m_MainPushConstant.DataOffset = cmd.Offset;
				s_Data->m_MainPushConstant.DirectionalLights = s_Data->m_DirectionalLightIndex;
				s_Data->m_MainPipeline->SubmitPushConstant(ShaderType::Vertex, s_Data->m_PushConstantSize, &s_Data->m_MainPushConstant);
				s_Data->m_MainPipeline->DrawMesh(cmd.Mesh, DrawMode::Triangle, cmd.InstancesCount);

				// resetting values
				cmd.InstancesCount = 0;
				cmd.Offset = 0;
				cmd.Mesh = nullptr;
			}
		}
		s_Data->m_MainPipeline->EndRenderPass();
		s_Data->m_MainPipeline->EndBufferSubmit();
	}

	void Renderer::StartNewBacth()
	{
		s_Data->m_Objects = 0;
		s_Data->m_InstanceDataIndex = 0;
		s_Data->m_DirectionalLightIndex = 0;
		s_Data->m_DrawListIndex = 0;
		s_Data->m_UsedMeshesIndex = 0;
	}

	void Renderer::SubmitMesh(const glm::vec3& pos, const glm::vec3& rotation,
		const glm::vec3& scale, const Ref<Mesh>& mesh)
	{
		if (s_Data->m_Objects >= s_Data->m_MaxObjects)
			StartNewBacth();

		auto& instance = s_Data->m_Packages[mesh.get()];
		if(instance.CurrentIndex >= s_MaxInstances)
			StartNewBacth();

		auto& package = instance.Data[instance.CurrentIndex];

		package.MaterialID = mesh->GetMaterialID();
		package.WorldPos = pos;
		package.Rotation = rotation;
		package.Scale = scale;
		instance.CurrentIndex++;

		s_Data->m_UsedMeshes[s_Data->m_UsedMeshesIndex] = mesh.get();
		s_Data->m_UsedMeshesIndex++;
		s_Data->m_Objects++;

		for (auto& sub : mesh->GetSubMeshes())
			SubmitMesh(pos, rotation, scale, sub);
	}

	void Renderer::SubmitDirectionalLight(const glm::vec3& pos, const glm::vec4& color)
	{
		uint32_t index = s_Data->m_DirectionalLightIndex;
		if (index >= s_MaxDirectionalLights)
			return; // temp;

		s_Data->m_DirectionalLights[index].Color = color;
		s_Data->m_DirectionalLights[index].Position = glm::vec4(pos, 1);
		s_Data->m_DirectionalLightIndex++;
	}

	void Renderer::SetAmbientMixer(float value)
	{
		s_Data->m_SceneData.Params.z = value;
	}

	void Renderer::SetGamma(float value)
	{
		s_Data->m_SceneData.Params.y = value;
	}

	void Renderer::SetExposure(float value)
	{
		s_Data->m_SceneData.Params.x = value;
	}

	void Renderer::InitPBR()
	{
#ifdef SMOLENGINE_OPENGL_IMPL
#else
		VulkanPBR::Init("../Resources/Textures/gcanyon_cube.ktx", TextureFormat::R16G16B16A16_SFLOAT);
#endif
	}

	void Renderer::InitPipelines()
	{
		// Main
		{
			s_Data->m_MainPipeline = std::make_unique<GraphicsPipeline>();
			GraphicsPipelineShaderCreateInfo shaderCI = {};
			{
				shaderCI.FilePaths[ShaderType::Vertex] = "../Resources/Shaders/PBR_Vulkan_Vertex.glsl";
				shaderCI.FilePaths[ShaderType::Fragment] = "../Resources/Shaders/PBR_Vulkan_Fragment.glsl";

				shaderCI.StorageBuffersSizes[25] = { sizeof(InstancesData) * s_InstanceDataMaxCount };
				shaderCI.StorageBuffersSizes[26] = { sizeof(Material) * 1000 };
				shaderCI.StorageBuffersSizes[28] = { sizeof(DirectionalLightBuffer) * s_MaxDirectionalLights };
			};

			BufferLayout mainLayout =
			{
				{ DataTypes::Float3, "aPos" },
				{ DataTypes::Float3, "aNormal" },
				{ DataTypes::Float4, "aTangent" },
				{ DataTypes::Float2, "aUV" },
				{ DataTypes::Float4, "aColor" }
			};

			VertexInputInfo vertexMain(sizeof(PBRVertex), mainLayout);

			GraphicsPipelineCreateInfo DynamicPipelineCI = {};
			{
				DynamicPipelineCI.VertexInputInfos = { vertexMain };
				DynamicPipelineCI.PipelineName = "PBR_Pipeline";
				DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
				DynamicPipelineCI.TargetFramebuffer = s_Data->m_Framebuffer;
			}

			auto result = s_Data->m_MainPipeline->Create(&DynamicPipelineCI);
			assert(result == PipelineCreateResult::SUCCESS);
#ifndef SMOLENGINE_OPENGL_IMPL
			s_Data->m_MainPipeline->UpdateVulkanImageDescriptor(2, VulkanPBR::GetIrradianceImageInfo());
			s_Data->m_MainPipeline->UpdateVulkanImageDescriptor(3, VulkanPBR::GetBRDFLUTImageInfo());
			s_Data->m_MainPipeline->UpdateVulkanImageDescriptor(4, VulkanPBR::GetPrefilteredCubeImageInfo());
#endif
		}

		// Sky Box
		{
			s_Data->m_SkyboxPipeline = std::make_unique<GraphicsPipeline>();
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
				DynamicPipelineCI.TargetFramebuffer = s_Data->m_Framebuffer;
			}

			auto result = s_Data->m_SkyboxPipeline->Create(&DynamicPipelineCI);
			assert(result == PipelineCreateResult::SUCCESS);
#ifndef SMOLENGINE_OPENGL_IMPL
			s_Data->m_SkyboxPipeline->UpdateVulkanImageDescriptor(1, VulkanPBR::GetSkyBox().GetVkDescriptorImageInfo());
#endif


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
			s_Data->m_SkyboxPipeline->SetVertexBuffers({ skyBoxFB });
		}
	}

	void Renderer::InitFramebuffers()
	{
		FramebufferSpecification framebufferCI = {};
		{
			framebufferCI.Width = Application::GetApplication().GetWindowWidth();
			framebufferCI.Height = Application::GetApplication().GetWindowHeight();
			framebufferCI.bUseMSAA = true;
			framebufferCI.bTargetsSwapchain = true;
			framebufferCI.Attachments = { FramebufferAttachment(AttachmentFormat::Color) };

			s_Data->m_Framebuffer = Framebuffer::Create(framebufferCI);
		}
	}

	bool Renderer::OnNewLevelLoaded()
	{
		return false;
	}

	bool Renderer::UpdateMaterials()
	{
		if (!s_Data->m_IsInitialized)
			return false;

		s_Data->m_MainPipeline->UpdateSamplers(MaterialLibrary::GetSinglenton()->GetTextures(), s_Data->m_TexturesBinding);
		void* data = nullptr;
		uint32_t size = 0;
		MaterialLibrary::GetSinglenton()->GetMaterialsPtr(data, size);
		s_Data->m_MainPipeline->SubmitStorageBuffer(s_Data->m_MaterialsBinding, size, data);

		return true;
	}

	Ref<Framebuffer> Renderer::GetFramebuffer()
	{
		return s_Data->m_Framebuffer;
	}

}