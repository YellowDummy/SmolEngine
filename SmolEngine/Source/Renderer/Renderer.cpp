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

#ifdef SMOLENGINE_OPENGL_IMPL
#include "Renderer/OpenGL/OpenglShader.h"
#include "Renderer/OpenGL/OpenglRendererAPI.h"
#else
#include "Renderer/Vulkan/VulkanPBR.h"
#endif

#include "ECS/Systems/CommandSystem.h"

namespace SmolEngine
{
	static const size_t s_MaxInstances = 200;
	static const size_t s_MaxPackages = 1200;
	static const size_t s_ShaderDataMaxCount = s_MaxPackages * s_MaxInstances;

	struct DrawElement
	{
		uint32_t                        m_Count = 0;
		uint32_t                        m_DataOffset = 0;
		Mesh*                           m_Mesh = nullptr;
	};

	struct ShaderData
	{
		glm::mat4                       Model = glm::mat4(0.0f);
		glm::vec4                       AddData = glm::vec4(0);
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

		uint32_t                         m_CurrentIndex = 0;
		std::array<Package,
			s_MaxPackages>               m_Data;
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
		uint32_t                         m_ShaderDataIndex = 0;
		uint32_t                         m_UsedMeshesIndex = 0;
		uint32_t                         m_DrawListIndex = 0;
		uint32_t                         m_MaxObjects = s_MaxPackages;

		Mesh*                            m_UsedMeshes[s_MaxPackages];
		ShaderData                       m_ShaderData[s_ShaderDataMaxCount];
		DrawElement                      m_DrawList[s_MaxPackages];

		std::unordered_map<Mesh*,
			InstancePackage>             m_Packages;

		// UBO's & Push Constants
		struct SceneData
		{
			glm::mat4                    Projection = glm::mat4(1.0f);
			glm::mat4                    View = glm::mat4(1.0f);
			glm::vec3                    CamPos = glm::vec3(1.0f);
		};

		struct PushConstant
		{
			int                          DataOffset = 0;
		};

		SceneData                        m_SceneData = {};
		PushConstant                     m_MainPushConstant = {};

		const size_t                     SceneDataSize = sizeof(SceneData);
		const size_t                     PushConstantSize = sizeof(PushConstant);
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
		s_Data->m_SceneData.CamPos = camPos;

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

	void Renderer::Flush()
	{
		for (uint32_t i = 0; i < s_Data->m_UsedMeshesIndex; ++i)
		{
			// getting values
			auto& drawElement = s_Data->m_DrawList[s_Data->m_DrawListIndex];
			Mesh* mesh = s_Data->m_UsedMeshes[i];
			auto& instance = s_Data->m_Packages[mesh];

			// setting draw list
			drawElement.m_DataOffset = s_Data->m_ShaderDataIndex;
			drawElement.m_Mesh = mesh;
			drawElement.m_Count = instance.m_CurrentIndex;

			for (uint32_t x = 0; x < instance.m_CurrentIndex; ++x)
			{
				auto& package = instance.m_Data[x];
				auto& shaderData = s_Data->m_ShaderData[s_Data->m_ShaderDataIndex];

				shaderData.AddData.x = package.MaterialID;
				CommandSystem::ComposeTransform(package.WorldPos, package.Rotation, package.Scale, true, shaderData.Model);

				s_Data->m_ShaderDataIndex++;
			}

			// resetting values
			instance.m_CurrentIndex = 0;

			s_Data->m_DrawListIndex++;
		}

		s_Data->m_MainPipeline->BeginBufferSubmit();
		{
			// Updates model views and material indexes
			s_Data->m_MainPipeline->SubmitStorageBuffer(s_Data->m_ShaderDataBinding, sizeof(ShaderData) * s_Data->m_ShaderDataIndex, &s_Data->m_ShaderData);

#ifdef SMOLENGINE_EDITOR
			// Update materials
			void* data = nullptr;
			uint32_t size = 0;
			MaterialLibrary::GetSinglenton()->GetMaterialsPtr(data, size);
			s_Data->m_MainPipeline->SubmitStorageBuffer(s_Data->m_MaterialsBinding, size, data);
#endif
			// Updates scene data
			s_Data->m_MainPipeline->SubmitUniformBuffer(s_Data->m_SceneDataBinding, s_Data->SceneDataSize, &s_Data->m_SceneData);
		}

		s_Data->m_MainPipeline->BeginRenderPass();
		{
			for (uint32_t i = 0; i < s_Data->m_DrawListIndex; ++i)
			{
				auto& element = s_Data->m_DrawList[i];

				s_Data->m_MainPushConstant.DataOffset = element.m_DataOffset;
				s_Data->m_MainPipeline->SubmitPushConstant(ShaderType::Vertex, s_Data->PushConstantSize, &s_Data->m_MainPushConstant);
				s_Data->m_MainPipeline->DrawMesh(element.m_Mesh, DrawMode::Triangle, element.m_Count);

				// resetting values
				element.m_Count = 0;
				element.m_DataOffset = 0;
				element.m_Mesh = nullptr;
			}
		}
		s_Data->m_MainPipeline->EndRenderPass();
		s_Data->m_MainPipeline->EndBufferSubmit();
	}

	void Renderer::StartNewBacth()
	{
		s_Data->m_Objects = 0;
		s_Data->m_ShaderDataIndex = 0;
		s_Data->m_DrawListIndex = 0;
		s_Data->m_UsedMeshesIndex = 0;
	}

	void Renderer::SubmitMesh(const glm::vec3& pos, const glm::vec3& rotation,
		const glm::vec3& scale, const Ref<Mesh>& mesh)
	{
		if (s_Data->m_Objects >= s_Data->m_MaxObjects)
			StartNewBacth();

		auto& instance = s_Data->m_Packages[mesh.get()];
		if(instance.m_CurrentIndex >= s_MaxInstances)
			StartNewBacth();

		auto& package = instance.m_Data[instance.m_CurrentIndex];

		package.MaterialID = mesh->GetMaterialID();
		package.WorldPos = pos;
		package.Rotation = rotation;
		package.Scale = scale;

		s_Data->m_UsedMeshes[s_Data->m_UsedMeshesIndex] = mesh.get();

		instance.m_CurrentIndex++;
		s_Data->m_UsedMeshesIndex++;
		s_Data->m_Objects++;

		for (auto& sub : mesh->GetSubMeshes())
			SubmitMesh(pos, rotation, scale, sub);
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

				shaderCI.StorageBuffersSizes[25] = { sizeof(ShaderData) * s_ShaderDataMaxCount };
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

			struct PBRParams
			{
				glm::vec4 lights = glm::vec4(-15.0, -15 * 0.5f, -15, 1.0f);
				glm::vec4 lightColors = glm::vec4(0.2f, 0.5f, 0.2f, 1.0f);
				float exposure = 2.5;
				float gamm = 4;

			} tempData;

			s_Data->m_MainPipeline->SubmitUniformBuffer(12, sizeof(PBRParams), &tempData);
#ifndef SMOLENGINE_OPENGL_IMPL
			s_Data->m_MainPipeline->UpdateVulkanImageDescriptor(2, VulkanPBR::GetIrradianceImageInfo());
			s_Data->m_MainPipeline->UpdateVulkanImageDescriptor(3, VulkanPBR::GetBRDFLUTImageInfo());
			s_Data->m_MainPipeline->UpdateVulkanImageDescriptor(4, VulkanPBR::GetPrefilteredCubeImageInfo());
#endif
		}

		return; // temp

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
				DynamicPipelineCI.VertexInputInfos = { VertexInputInfo(sizeof(SkyBoxData), layout, false) };
				DynamicPipelineCI.PipelineName = "Skybox_Test";
				DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
			}

			auto result = s_Data->m_SkyboxPipeline->Create(&DynamicPipelineCI);
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
			framebufferCI.Attachments = { FramebufferAttachment(AttachmentFormat::Color, true) };

			s_Data->m_Framebuffer = Framebuffer::Create(framebufferCI);
		}
	}
}