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

#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/gtx/dual_quaternion.hpp>

namespace SmolEngine
{
#define SHADOW_MAP_CASCADE_COUNT 4

	static const size_t                  s_MaxInstances = 500;
	static const size_t                  s_MaxPackages = 1200;
	static const size_t                  s_MaxDirectionalLights = 1000;
	static const size_t                  s_MaxPointLights = 1000;
	static const size_t                  s_InstanceDataMaxCount = s_MaxPackages * s_MaxInstances;

	struct CommandBuffer
	{
		uint32_t                         InstancesCount = 0;
		uint32_t                         Offset = 0;
		Mesh*                            Mesh = nullptr;
	};									 
										 
	struct InstanceData
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

		// States
		bool                             m_IsInitialized = false;
		bool                             m_ShowDebugView = false;

		// Bindings
		const uint32_t                   m_TexturesBinding = 24;
		const uint32_t                   m_ShaderDataBinding = 25;
		const uint32_t                   m_MaterialsBinding = 26;
		const uint32_t                   m_SceneDataBinding = 27;

		// Instance Data
		uint32_t                         m_Objects = 0;
		uint32_t                         m_InstanceDataIndex = 0;
		uint32_t                         m_UsedMeshesIndex = 0;
		uint32_t                         m_DrawListIndex = 0;
		uint32_t                         m_DirectionalLightIndex = 0;
		uint32_t                         m_PointLightIndex = 0;
		uint32_t                         m_MaxObjects = s_MaxPackages;

		// Pipelines
		Scope<GraphicsPipeline>          m_MainPipeline = nullptr;
		Scope<GraphicsPipeline>          m_SkyboxPipeline = nullptr;
		Scope<GraphicsPipeline>          m_OmniPipeline = nullptr;
		Scope<GraphicsPipeline>          m_DepthPassPipeline = nullptr;
		Scope<GraphicsPipeline>          m_DebugViewPipeline = nullptr;

		// Framebuffers
		Ref<Framebuffer>                 m_Framebuffer = nullptr;
		Ref<Framebuffer>                 m_DepthFramebuffer = nullptr;
		Ref<Framebuffer>                 m_OmniFramebuffer = nullptr;

		// Buffers
		Mesh*                            m_UsedMeshes[s_MaxPackages];
		InstanceData                     m_InstancesData[s_InstanceDataMaxCount];
		CommandBuffer                    m_DrawList[s_MaxPackages];
		DirectionalLightBuffer           m_DirectionalLights[s_MaxDirectionalLights];
		PointLightBuffer                 m_PointLights[s_MaxPointLights];

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

		float                            m_NearClip = 1.0f;
		float                            m_FarClip = 1.0f;
		glm::vec3                        m_ShadowLightDirection = glm::vec3(0.0f, 0.0f, 0.0f);

		struct AmbientLighting
		{
			glm::vec4                    DiffuseColor = glm::vec4(1.0f);
			glm::vec4                    SpecularColor = glm::vec4(1.0f);
			glm::vec4                    Ambient = glm::vec4(1.0f);
			glm::vec4                    Params = glm::vec4(1.0f, 1.0f, 0.0f, 0.0f); // x = IBL scale, y = enable IBL;
		};

		struct PushConstant
		{
			glm::mat4                    DepthMVP = glm::mat4(1.0f);

			uint32_t                     DataOffset = 0;
			uint32_t                     DirectionalLights = 0;
			uint32_t                     PointLights = 0;
		};

		struct DebugView
		{
			uint32_t                     ShowOmniCube = 0;
			uint32_t                     ShowMRT = 0;		                 
			uint32_t                     MRTattachmentIndex = 0;
		};

		struct ShadowMatrix
		{
			glm::mat4 shadowTransforms[6];
		};

		DebugView                        m_DebugView = {};
		SceneData                        m_SceneData = {};
		PushConstant                     m_MainPushConstant = {};
		ShadowMatrix                     m_ShadowMatrix = {};
		AmbientLighting                  m_AmbientLighting = {};

		// Sizes
		const size_t                     m_DebugViewSize = sizeof(DebugView);
		const size_t                     m_SceneDataSize = sizeof(SceneData);
		const size_t                     m_PushConstantSize = sizeof(PushConstant);
		const size_t                     m_AmbientLightingSize = sizeof(AmbientLighting);
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
		s_Data->m_IsInitialized = false;
		delete s_Data;
	}

	void Renderer::BeginScene(const BeginSceneInfo& info)
	{
		s_Data->m_SceneData.View = info.view;
		s_Data->m_SceneData.Projection = info.proj;
		s_Data->m_SceneData.CamPos = glm::vec4(info.pos, 1);
		s_Data->m_SceneData.SkyBoxMatrix = glm::mat4(glm::mat3(info.view));
		s_Data->m_NearClip = info.nearClip;
		s_Data->m_FarClip = info.farClip;

		s_Data->m_MainPipeline->BeginCommandBuffer(true);
		s_Data->m_SkyboxPipeline->BeginCommandBuffer(true);

		// Clear Pass
		s_Data->m_MainPipeline->BeginRenderPass();
		{
			s_Data->m_MainPipeline->ClearColors();
		}
		s_Data->m_MainPipeline->EndRenderPass();

		Reset();
	}

	void Renderer::EndScene()
	{
		Flush();
		s_Data->m_SkyboxPipeline->EndCommandBuffer();
		s_Data->m_MainPipeline->EndCommandBuffer();
	}

	void Renderer::Flush()
	{
		for (uint32_t i = 0; i < s_Data->m_UsedMeshesIndex; ++i)
		{
			// Getting values
			Mesh* mesh = s_Data->m_UsedMeshes[i];
			auto& instance = s_Data->m_Packages[mesh];
			auto& cmd = s_Data->m_DrawList[s_Data->m_DrawListIndex];

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
			s_Data->m_MainPipeline->SubmitBuffer(s_Data->m_SceneDataBinding, s_Data->m_SceneDataSize, &s_Data->m_SceneData);

			if (s_Data->m_DirectionalLightIndex > 0)
			{
				// Updates Directional Lights
				s_Data->m_MainPipeline->SubmitBuffer(28, sizeof(DirectionalLightBuffer) * s_Data->m_DirectionalLightIndex, &s_Data->m_DirectionalLights);
				// Calculate Depth
				s_Data->m_MainPushConstant.DepthMVP = CalculateDepthMVP(s_Data->m_ShadowLightDirection);
			}

			if (s_Data->m_PointLightIndex > 0)
			{
				// Updates Point Lights
				s_Data->m_MainPipeline->SubmitBuffer(29, sizeof(PointLightBuffer) * s_Data->m_PointLightIndex, &s_Data->m_PointLights);
			}

			// Updates Ambient Lighting
			s_Data->m_MainPipeline->SubmitBuffer(30, s_Data->m_AmbientLightingSize, &s_Data->m_AmbientLighting);

			// Updates model views and material indexes
			s_Data->m_MainPipeline->SubmitBuffer(s_Data->m_ShaderDataBinding, sizeof(InstanceData) * s_Data->m_InstanceDataIndex, &s_Data->m_InstancesData);

#ifdef SMOLENGINE_EDITOR
			// Update materials
			UpdateMaterials();
#endif
		}

		// Depth Pass
		if (s_Data->m_DirectionalLightIndex > 0)
		{
			s_Data->m_DepthPassPipeline->BeginCommandBuffer(true);
#ifndef SMOLENGINE_OPENGL_IMPL
			VkCommandBuffer cmdBuffer = s_Data->m_DepthPassPipeline->GetVkCommandBuffer();
#endif
			s_Data->m_DepthPassPipeline->BeginRenderPass();
			{
				// Set depth bias (aka "Polygon offset")
				// Required to avoid shadow mapping artifacts
				vkCmdSetDepthBias(cmdBuffer, 1.25f, 0.0f, 1.75f);

				struct PushConstant
				{
					glm::mat4 depthMVP;
					uint32_t offset;

				} static pc;

				pc.depthMVP = s_Data->m_MainPushConstant.DepthMVP;

				for (uint32_t i = 0; i < s_Data->m_DrawListIndex; ++i)
				{
					auto& cmd = s_Data->m_DrawList[i];
					pc.offset = cmd.Offset;

					s_Data->m_DepthPassPipeline->SubmitPushConstant(ShaderType::Vertex, sizeof(PushConstant), &pc);
					s_Data->m_DepthPassPipeline->DrawMesh(cmd.Mesh, DrawMode::Triangle, cmd.InstancesCount);
				}
			}
			s_Data->m_DepthPassPipeline->EndRenderPass();

		}

		// Debug Pass
		if (s_Data->m_ShowDebugView)
		{
			s_Data->m_DebugViewPipeline->BeginCommandBuffer(true);
			s_Data->m_DebugViewPipeline->BeginRenderPass();
			{
				s_Data->m_DebugViewPipeline->SubmitPushConstant(ShaderType::Fragment, s_Data->m_DebugViewSize, &s_Data->m_DebugView);
				s_Data->m_DebugViewPipeline->DrawIndexed();
			}
			s_Data->m_DebugViewPipeline->EndRenderPass();
			return;
		}

		// SkyBox
		s_Data->m_SkyboxPipeline->BeginRenderPass();
		{
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
				s_Data->m_MainPushConstant.PointLights = s_Data->m_PointLightIndex;

				s_Data->m_MainPipeline->SubmitPushConstant(ShaderType::Vertex, s_Data->m_PushConstantSize, &s_Data->m_MainPushConstant);
				s_Data->m_MainPipeline->DrawMesh(cmd.Mesh, DrawMode::Triangle, cmd.InstancesCount);
			}
		}
		s_Data->m_MainPipeline->EndRenderPass();
		s_Data->m_MainPipeline->EndBufferSubmit();

		// Post-Processing
	}

	void Renderer::StartNewBacth()
	{
		Flush();
		Reset();
	}

	void Renderer::SubmitMesh(const glm::vec3& pos, const glm::vec3& rotation,
		const glm::vec3& scale, const Ref<Mesh>& mesh, int32_t materialID)
	{
		if (s_Data->m_Objects >= s_Data->m_MaxObjects)
			StartNewBacth();

		auto& instance = s_Data->m_Packages[mesh.get()];
		if(instance.CurrentIndex >= s_MaxInstances)
			StartNewBacth();

		auto& package = instance.Data[instance.CurrentIndex];

		package.MaterialID = materialID == -1 ? mesh->GetMaterialID(): materialID;
		package.WorldPos = pos;
		package.Rotation = rotation;
		package.Scale = scale;
		instance.CurrentIndex++;

		bool found = false;
		for (uint32_t i = 0; i < s_Data->m_UsedMeshesIndex; ++i)
		{
			if (s_Data->m_UsedMeshes[i] == mesh.get())
			{
				found = true;
				break;
			}
		}

		if (found == false)
		{
			s_Data->m_UsedMeshes[s_Data->m_UsedMeshesIndex] = mesh.get();
			s_Data->m_UsedMeshesIndex++;
		}
		s_Data->m_Objects++;

		for (auto& sub : mesh->GetSubMeshes())
			SubmitMesh(pos, rotation, scale, sub);
	}

	void Renderer::SubmitDirectionalLight(const glm::vec3& dir, const glm::vec4& color)
	{
		uint32_t index = s_Data->m_DirectionalLightIndex;
		if (index >= s_MaxDirectionalLights)
			return; // temp;

		s_Data->m_DirectionalLights[index].Color = color;
		s_Data->m_DirectionalLights[index].Position = glm::vec4(dir, 1.0f);
		s_Data->m_DirectionalLightIndex++;
	}

	void Renderer::SubmitPointLight(const glm::vec3& pos, const glm::vec4& color, float constant, float linear, float exp)
	{
		uint32_t index = s_Data->m_PointLightIndex;
		if (index >= s_MaxPointLights)
			return; // temp;

		s_Data->m_PointLights[index].Color = color;
		s_Data->m_PointLights[index].Position = glm::vec4(pos, 1.0f);
		s_Data->m_PointLights[index].Params.x = constant;
		s_Data->m_PointLights[index].Params.y = linear;
		s_Data->m_PointLights[index].Params.z = exp;
		s_Data->m_PointLightIndex++;
	}

	void Renderer::SetDebugViewParams(DebugViewInfo& info)
	{
		s_Data->m_DebugView.ShowOmniCube = info.bShowOmniCube;
		s_Data->m_DebugView.ShowMRT = info.bShowMRT;
		s_Data->m_DebugView.MRTattachmentIndex = info.mrtAttachmentIndex;
	}

	void Renderer::SetAmbientLighting(const glm::vec3& diffuseColor, glm::vec3& specularColor, float IBLscale, bool enableIBL, glm::vec3& ambient)
	{
		s_Data->m_AmbientLighting.DiffuseColor = glm::vec4(diffuseColor, 0.0f);
		s_Data->m_AmbientLighting.SpecularColor = glm::vec4(specularColor, 0.0f);
		s_Data->m_AmbientLighting.Ambient = glm::vec4(ambient, 0.0f);
		s_Data->m_AmbientLighting.Params.x = IBLscale;
		s_Data->m_AmbientLighting.Params.y = enableIBL ? 1.0f : 0.0f;
	}

	void Renderer::SetShadowLightDirection(const glm::vec3& dir)
	{
		s_Data->m_ShadowLightDirection = dir;
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

	void Renderer::SetActiveDebugView(bool value)
	{
		s_Data->m_ShowDebugView = value;
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
		BufferLayout mainLayout =
		{
			{ DataTypes::Float3, "aPos" },
			{ DataTypes::Float3, "aNormal" },
			{ DataTypes::Float4, "aTangent" },
			{ DataTypes::Float2, "aUV" },
			{ DataTypes::Int4,   "aBoneIDs"},
			{ DataTypes::Float4, "aWeight"}
		};

		VertexInputInfo vertexMain(sizeof(PBRVertex), mainLayout);

		// Main
		{
			s_Data->m_MainPipeline = std::make_unique<GraphicsPipeline>();
			GraphicsPipelineShaderCreateInfo shaderCI = {};
			{
				shaderCI.FilePaths[ShaderType::Vertex] = "../Resources/Shaders/Vulkan/PBR.vert";
				shaderCI.FilePaths[ShaderType::Fragment] = "../Resources/Shaders/Vulkan/PBR.frag";

				shaderCI.StorageBuffersSizes[25] = { sizeof(InstanceData) * s_InstanceDataMaxCount };
				shaderCI.StorageBuffersSizes[26] = { sizeof(Material) * 1000 };
				shaderCI.StorageBuffersSizes[28] = { sizeof(DirectionalLightBuffer) * s_MaxDirectionalLights };
				shaderCI.StorageBuffersSizes[29] = { sizeof(PointLightBuffer) * s_MaxPointLights };
			};

			GraphicsPipelineCreateInfo DynamicPipelineCI = {};
			{
				DynamicPipelineCI.VertexInputInfos = { vertexMain };
				DynamicPipelineCI.PipelineName = "PBR_Pipeline";
				DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
				DynamicPipelineCI.TargetFramebuffer = s_Data->m_Framebuffer;
			}

			auto result = s_Data->m_MainPipeline->Create(&DynamicPipelineCI);
			assert(result == PipelineCreateResult::SUCCESS);

			s_Data->m_MainPipeline->SubmitBuffer(30, s_Data->m_AmbientLightingSize, &s_Data->m_AmbientLighting);
#ifndef SMOLENGINE_OPENGL_IMPL
			s_Data->m_MainPipeline->UpdateVulkanImageDescriptor(1, s_Data->m_DepthFramebuffer->GetVulkanFramebuffer().GetDethAttachment()->ImageInfo);
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
				shaderCI.FilePaths[ShaderType::Vertex] = "../Resources/Shaders/Vulkan/Skybox.vert";
				shaderCI.FilePaths[ShaderType::Fragment] = "../Resources/Shaders/Vulkan/Skybox.frag";
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
				DynamicPipelineCI.PipelineName = "Skybox_Pipiline";
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

		// Debug View
		{
			s_Data->m_DebugViewPipeline = std::make_unique<GraphicsPipeline>();
			GraphicsPipelineShaderCreateInfo shaderCI = {};
			{
				shaderCI.FilePaths[ShaderType::Vertex] = "../Resources/Shaders/Vulkan/GenVertex.vert";
				shaderCI.FilePaths[ShaderType::Fragment] = "../Resources/Shaders/Vulkan/DebugView.frag";
			};

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

			GraphicsPipelineCreateInfo DynamicPipelineCI = {};
			{
				DynamicPipelineCI.VertexInputInfos = { VertexInputInfo(sizeof(FullSreenData), FullSreenlayout) };
				DynamicPipelineCI.PipelineName = "DebugView_Pipeline";
				DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
				DynamicPipelineCI.TargetFramebuffer = s_Data->m_Framebuffer;
				DynamicPipelineCI.PipelineCullMode = CullMode::None;

				auto result = s_Data->m_DebugViewPipeline->Create(&DynamicPipelineCI);
				assert(result == PipelineCreateResult::SUCCESS);
#ifndef SMOLENGINE_OPENGL_IMPL
				s_Data->m_DebugViewPipeline->UpdateVulkanImageDescriptor(1, s_Data->m_DepthFramebuffer->GetVulkanFramebuffer().GetDethAttachment()->ImageInfo);
#endif

				s_Data->m_DebugViewPipeline->SetVertexBuffers({ FullScreenVB });
				s_Data->m_DebugViewPipeline->SetIndexBuffers({ FullScreenID });
			}
		}

		// Depth Pass
		{
			s_Data->m_DepthPassPipeline = std::make_unique<GraphicsPipeline>();
			GraphicsPipelineShaderCreateInfo shaderCI = {};
			{
				shaderCI.FilePaths[ShaderType::Vertex] = "../Resources/Shaders/Vulkan/DepthPass.vert";
				shaderCI.FilePaths[ShaderType::Fragment] = "../Resources/Shaders/Vulkan/DepthPass.frag";
			};

			GraphicsPipelineCreateInfo DynamicPipelineCI = {};
			{
				DynamicPipelineCI.VertexInputInfos = { vertexMain };
				DynamicPipelineCI.PipelineName = "DepthPass_Pipeline";
				DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
				DynamicPipelineCI.TargetFramebuffer = s_Data->m_DepthFramebuffer;
				DynamicPipelineCI.bDepthBiasEnabled = true;
				DynamicPipelineCI.StageCount = 1;

				auto result = s_Data->m_DepthPassPipeline->Create(&DynamicPipelineCI);
				assert(result == PipelineCreateResult::SUCCESS);
			}
		}

		return;

		// Omni Pass
		{
			s_Data->m_OmniPipeline = std::make_unique<GraphicsPipeline>();
			GraphicsPipelineShaderCreateInfo shaderCI = {};
			{
				shaderCI.FilePaths[ShaderType::Vertex] = "../Resources/Shaders/Vulkan/OmniShadow.vert";
				shaderCI.FilePaths[ShaderType::Fragment] = "../Resources/Shaders/Vulkan/OmniShadow.frag";
			};

			GraphicsPipelineCreateInfo DynamicPipelineCI = {};
			{
				DynamicPipelineCI.VertexInputInfos = { vertexMain };
				DynamicPipelineCI.PipelineName = "OmniPass_Pipeline";
				DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
				DynamicPipelineCI.TargetFramebuffer = s_Data->m_OmniFramebuffer;

				auto result = s_Data->m_OmniPipeline->Create(&DynamicPipelineCI);
				assert(result == PipelineCreateResult::SUCCESS);
			}
		}
	}

	void Renderer::InitFramebuffers()
	{
		// Main
		{
			s_Data->m_Framebuffer = GraphicsContext::GetSingleton()->GetFramebuffer();
		}

		// Depth
		{
			FramebufferSpecification framebufferCI = {};
			{
				framebufferCI.Width = 4096;
				framebufferCI.Height = 4096;
				framebufferCI.Specialisation = FramebufferSpecialisation::ShadowMap;

				s_Data->m_DepthFramebuffer = Framebuffer::Create(framebufferCI);
			}
		}

		return;

		// Omni
		{
			FramebufferSpecification framebufferCI = {};
			{
				framebufferCI.Width = 1024;
				framebufferCI.Height = 1024;
				framebufferCI.Specialisation = FramebufferSpecialisation::OmniShadow;
				framebufferCI.NumSubpassDependencies = 0;

				s_Data->m_OmniFramebuffer = Framebuffer::Create(framebufferCI);
			}
		}
	}

	glm::mat4 Renderer::CalculateDepthMVP(const glm::vec3& lightPos)
	{
		// Keep depth range as small as possible
		// for better shadow map precision
		float zNear = 1.0f;
		float zFar = 200.0f;
		float lightFOV = 45.0f;

		// Matrix from light's point of view
		glm::mat4 depthProjectionMatrix = glm::perspective(lightFOV, 1.0f, zNear, zFar);
		glm::mat4 depthViewMatrix = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0, 1, 0));
		glm::mat4 depthModelMatrix = glm::mat4(1.0f);

		return depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
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
		s_Data->m_MainPipeline->SubmitBuffer(s_Data->m_MaterialsBinding, size, data);

		return true;
	}

	void Renderer::Reset()
	{
		s_Data->m_Objects = 0;
		s_Data->m_InstanceDataIndex = 0;
		s_Data->m_DirectionalLightIndex = 0;
		s_Data->m_PointLightIndex = 0;
		s_Data->m_DrawListIndex = 0;
		s_Data->m_UsedMeshesIndex = 0;
	}

	Ref<Framebuffer> Renderer::GetFramebuffer()
	{
		return s_Data->m_Framebuffer;
	}

}