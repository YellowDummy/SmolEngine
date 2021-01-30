#include "stdafx.h"
#include "Renderer.h"

#include "Renderer/VertexArray.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/GraphicsPipeline.h"
#include "Renderer/Shader.h"

#ifdef SMOLENGINE_OPENGL_IMPL
#include "Renderer/OpenGL/OpenglShader.h"
#include "Renderer/OpenGL/OpenglRendererAPI.h"
#else
#include "Renderer/Vulkan/VulkanPBR.h"
#endif

namespace SmolEngine
{
	struct RendererData
	{
		Ref<GraphicsPipeline> m_MainPipeline = nullptr;
		Ref<GraphicsPipeline> m_SkyboxPipeline = nullptr;
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

	void Renderer::SubmitMesh(const glm::vec3& pos, const glm::vec3& rotation,
		const glm::vec3& scale, const Ref<Mesh>& mesh, const MaterialComponent& material)
	{

	}

	void Renderer::SubmitCube(const glm::vec3& pos, const glm::vec3& rotation,
		const glm::vec3& scale, const MaterialComponent& material)
	{

	}

	void Renderer::SubmitSphere(const glm::vec3& pos, const glm::vec3& rotation,
		const glm::vec3& scale, const MaterialComponent& material)
	{

	}

	void Renderer::SubmitPlane(const glm::vec3& pos, const glm::vec3& rotation,
		const glm::vec3& scale, const MaterialComponent& material)
	{

	}

	void Renderer::SubmitCapsule(const glm::vec3& pos, const glm::vec3& rotation,
		const glm::vec3& scale, const MaterialComponent& material)
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
			{ ShaderDataType::Float3, "aPos" },
			{ ShaderDataType::Float3, "aNormal" },
			{ ShaderDataType::Float4, "aTangent" },
			{ ShaderDataType::Float2, "aUV" },
			{ ShaderDataType::Float4, "aColor" }
		};

		BufferLayout materialLayout =
		{
			{ ShaderDataType::Float, "aAlbedo" },
			{ ShaderDataType::Float, "aMetallic" },
			{ ShaderDataType::Float, "aRoughness" },
		};

		// Main vertex - no intance rate
		VertexInputInfo vertexMain(sizeof(PBRVertex), mainLayout);

		// Material vertex = instanced
		VertexInputInfo vertexMaterial(sizeof(PBRMaterialVertex), materialLayout, true);

		GraphicsPipelineCreateInfo DynamicPipelineCI = {};
		{
			DynamicPipelineCI.VertexInputInfos = { vertexMain };
			DynamicPipelineCI.PipelineName = "PBR_TEST";
			DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
			DynamicPipelineCI.IsTargetsSwapchain = true;
			DynamicPipelineCI.IsDepthTestEnabled = true;
		}

		bool result = s_Data->m_MainPipeline->Create(&DynamicPipelineCI);
		assert(result == true);
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
			{ ShaderDataType::Float3, "aPos" }
		};

		GraphicsPipelineCreateInfo DynamicPipelineCI = {};
		{
			DynamicPipelineCI.VertexInputInfos = { VertexInputInfo(sizeof(SkyBoxData), layout, false) };
			DynamicPipelineCI.PipelineName = "Skybox_Test";
			DynamicPipelineCI.ShaderCreateInfo = &shaderCI;
			DynamicPipelineCI.IsTargetsSwapchain = true;
			DynamicPipelineCI.IsDepthTestEnabled = false;
		}

		bool result = s_Data->m_SkyboxPipeline->Create(&DynamicPipelineCI);
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
		s_Data->m_SkyboxPipeline->SetVertexBuffers({ skyBoxFB });
	}

}