#include "stdafx.h"
#include "Renderer.h"

#include "Renderer/VertexArray.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/GraphicsPipeline.h"
#include "Renderer/Shader.h"
#include "Renderer/Shared.h"

#ifdef SMOLENGINE_OPENGL_IMPL
#include "Renderer/OpenGL/OpenglShader.h"
#include "Renderer/OpenGL/OpenglRendererAPI.h"
#else
#include "Renderer/Vulkan/VulkanPBR.h"
#endif

namespace SmolEngine
{
	static const uint32_t s_MaxInstances = 8192;
	static const uint32_t s_MaxPackages = 12000;

	struct InstancePackage
	{
		Ref<Mesh>                        Mesh = nullptr;
		uint32_t                         Count = 0;
	};

	struct RendererData
	{
		RendererData()
		{
			Packages.reserve(s_MaxPackages);
		}

		// Pipelines
		Ref<GraphicsPipeline>            m_MainPipeline = nullptr;
		Ref<GraphicsPipeline>            m_SkyboxPipeline = nullptr;

		// Instance Data
		std::unordered_map<Ref<Mesh>,
			InstancePackage>             Packages;

		// Scene Data
		struct CurrentSceneData
		{
			glm::mat4                    ViewProjectionMatrix = glm::mat4(1.0);
			Ref<Framebuffer>             TargetFramebuffer = nullptr;
		}                                SceneData = {};
	};

	static RendererData* s_Data = nullptr;

	void Renderer::Init()
	{
		s_Data = new RendererData();

		InitPBR();
		InitMainPipeline();
	}

	void Renderer::Shutdown()
	{
		delete s_Data;
	}

	void Renderer::BeginScene(const glm::mat4& viewProj)
	{

	}

	void Renderer::EndScene()
	{

	}

	void Renderer::OnNewLevelLoaded()
	{

	}

	void Renderer::Flush()
	{

	}

	void Renderer::StartNewBacth()
	{

	}

	void Renderer::SubmitMesh(const glm::vec3& pos, const glm::vec3& rotation,
		const glm::vec3& scale, const Ref<Mesh>& mesh, const PBRMaterial& PBRmaterial)
	{

	}

	void Renderer::InitPBR()
	{
#ifdef SMOLENGINE_OPENGL_IMPL
#else
		VulkanPBR::Init("../Resources/Textures/gcanyon_cube.ktx", TextureFormat::R16G16B16A16_SFLOAT);
#endif
	}

	void Renderer::InitMainPipeline()
	{
		s_Data->m_MainPipeline = std::make_shared<GraphicsPipeline>();
		GraphicsPipelineShaderCreateInfo shaderCI = {};
		{
			shaderCI.FilePaths[ShaderType::Vertex] = "../Resources/Shaders/PBR_Vulkan_Vertex.glsl";
			shaderCI.FilePaths[ShaderType::Fragment] = "../Resources/Shaders/PBR_Vulkan_Fragment.glsl";
		};

		BufferLayout mainLayout =
		{
			{ DataTypes::Float3, "aPos" },
			{ DataTypes::Float3, "aNormal" },
			{ DataTypes::Float4, "aTangent" },
			{ DataTypes::Float2, "aUV" },
			{ DataTypes::Float4, "aColor" }
		};

		BufferLayout materialLayout =
		{
			{ DataTypes::Float, "aAlbedo" },
			{ DataTypes::Float, "aMetallic" },
			{ DataTypes::Float, "aRoughness" },
		};			  

		// Main vertex - no intance rate
		VertexInputInfo vertexMain(sizeof(PBRVertex), mainLayout);

		GraphicsPipelineCreateInfo DynamicPipelineCI = {};
		{
			DynamicPipelineCI.VertexInputInfos = { vertexMain };
			DynamicPipelineCI.PipelineName = "PBR_TEST";
			DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
		}

		auto result = s_Data->m_MainPipeline->Create(&DynamicPipelineCI);
		assert(result == PipelineCreateResult::SUCCESS);
	}

	void Renderer::InitSkyBoxPipeline()
	{
		s_Data->m_SkyboxPipeline = std::make_shared<GraphicsPipeline>();
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