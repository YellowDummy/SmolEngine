#pragma once

#include "GraphicsPipeline.h"

#include "Common/Framebuffer.h"
#include "Common/Texture.h"
#include "Common/Mesh.h"
#include "Common/RendererShared.h"
#include "Common/Animation.h"

#include "Utils/Frustum.h"
#include "Utils/GLM.h"

#include <mutex>

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
#pragma region Limits
	static const uint32_t      max_animations = 100;
	static const uint32_t      max_anim_joints = 1000;
	static const uint32_t      max_lights = 1000;
	static const uint32_t      max_objects = 15000;
#pragma endregion

#pragma region Shader Defs

	struct DirectionalLight
	{
		alignas(16) glm::vec4  Direction = glm::vec4(67, 56, 0, 0);
		alignas(16) glm::vec4  Color = glm::vec4(1.0);

		alignas(4) float       Intensity = 1.0f;
		alignas(4) float       Bias = 1.0f;
		alignas(4) float       zNear = 1.0f;
		alignas(4) float       zFar = 350.0f;
		alignas(4) float       lightFOV = 45.0f;
		alignas(4) uint32_t    IsActive = false;
		alignas(4) uint32_t    IsCastShadows = false;
		alignas(4) uint32_t    IsUseSoftShadows = true;
	};

	struct PointLight
	{
		alignas(16) glm::vec4  Position = glm::vec4(0);
		alignas(16) glm::vec4  Color = glm::vec4(1.0);

		alignas(4) float       Intensity = 1.0f;
		alignas(4) float       Raduis = 10.0f;
		alignas(4) float       Bias = 1.0f;
		alignas(4) uint32_t    IsActive = true;
		alignas(4) uint32_t    IsCastShadows = false;

	private:
		alignas(4) uint32_t    pad1 = 0;
		alignas(4) uint32_t    pad2 = 0;
		alignas(4) uint32_t    pad3 = 0;
	};

	struct SpotLight
	{
		alignas(16) glm::vec4  Position = glm::vec4(0, 0, 0, 0);
		alignas(16) glm::vec4  Direction = glm::vec4(0, 0, 40, 0);
		alignas(16) glm::vec4  Color = glm::vec4(1.0);

		alignas(4) float       Intensity = 10.0f;
		alignas(4) float       CutOff = 40.0f;
		alignas(4) float       OuterCutOff = 5.0f;
		alignas(4) float       Raduis = 10.0f;
		alignas(4) float       Bias = 0.005f;
		alignas(4) uint32_t    IsActive = true;
		alignas(4) uint32_t    IsCastShadows = false;

	private:
		alignas(4) uint32_t pad1 = 0;
	};

	struct InstanceData
	{
		alignas(4) uint32_t              MaterialID = 0;
		alignas(4) uint32_t              IsAnimated = false;
		alignas(4) uint32_t              AnimOffset = 0;
		alignas(4) uint32_t              EntityID = 0;

		alignas(16) glm::mat4            ModelView = glm::mat4(1.0f);
	};

	struct SceneState
	{
		alignas(4) float       HDRExposure = 1.0f;
		alignas(4) uint32_t    UseIBL = true;
	private:
		alignas(4) uint32_t    NumPointsLights = 0;
		alignas(4) uint32_t    NumSpotLights = 0;

		friend class DeferredRenderer;
	};

#pragma endregion

#pragma region Starages 

	enum class PostProcessingFlags : uint32_t
	{
		None,
		Bloom,
		Blur
	};

	enum class DebugViewFlags : uint32_t
	{
		None = 0,
		Albedro,
		Position,
		Normals,
		Materials,
		Emission,
		ShadowMap,
		ShadowMapCood,
		AO
	};

	struct RendererState
	{
		bool                   bDrawSkyBox = true;
		bool                   bDrawGrid = true;
		bool                   bHDR = true;
		bool                   bFXAA = true;
		bool                   bSSAO = false;
		PostProcessingFlags    ePostProcessing = PostProcessingFlags::Bloom;
		DebugViewFlags         eDebugView = DebugViewFlags::None;
		SceneState             SceneState = {};
	};

	struct CommandBuffer
	{
		uint32_t               InstancesCount = 0;
		uint32_t               Offset = 0;
		Mesh*                  Mesh = nullptr;
	};

	struct InstancePackage
	{
		struct Package
		{
			uint32_t           MaterialID = 0;
			glm::vec3*         WorldPos = nullptr;
			glm::vec3*         Rotation = nullptr;
			glm::vec3*         Scale = nullptr;
		};

		uint32_t               CurrentIndex = 0;
		std::vector<Package>   Packages;
	};

#pragma endregion

	struct RendererStorage
	{
		// States
		bool                                       m_IsInitialized = false;
		ShadowMapSize                              m_MapSize = ShadowMapSize::SIZE_8;
		// Bindings						           
		const uint32_t                             m_TexturesBinding = 24;
		const uint32_t                             m_ShaderDataBinding = 25;
		const uint32_t                             m_MaterialsBinding = 26;
		const uint32_t                             m_SceneDataBinding = 27;
		const uint32_t                             m_AnimBinding = 28;
		const uint32_t                             m_PointLightBinding = 30;
		const uint32_t                             m_SpotLightBinding = 31;
		const uint32_t                             m_DirLightBinding = 32;
		const uint32_t                             m_SceneStateBinding = 33;
		// Instance Data				           
		uint32_t                                   m_Objects = 0;
		uint32_t                                   m_InstanceDataIndex = 0;
		uint32_t                                   m_PointLightIndex = 0;
		uint32_t                                   m_SpotLightIndex = 0;
		uint32_t                                   m_LastAnimationOffset = 0;
		// Pipelines					           
		GraphicsPipeline                           m_GbufferPipeline = {};
		GraphicsPipeline                           m_LightingPipeline = {};
		GraphicsPipeline                           m_BloomPipeline = {};
		GraphicsPipeline                           m_BlurPipeline = {};
		GraphicsPipeline                           m_CombinationPipeline = {};
		GraphicsPipeline                           m_SkyboxPipeline = {};
		GraphicsPipeline                           m_OmniPipeline = {};
		GraphicsPipeline                           m_DepthPassPipeline = {};
		GraphicsPipeline                           m_GridPipeline = {};
		GraphicsPipeline                           m_DebugPipeline = {};
		GraphicsPipeline                           m_HDRPipeline = {};
		//Meshes						           
		Mesh                                       m_PlaneMesh = {};
		// Framebuffers					           
		Framebuffer*                               m_MainFramebuffer = nullptr;
		Framebuffer                                m_GFramebuffer = {};
		Framebuffer                                m_LightingFramebuffer = {};
		Framebuffer                                m_HDRFramebuffer = {};
		Framebuffer                                m_PostProcessingFramebuffer = {};
		Framebuffer                                m_DepthFramebuffer = {};
		// Buffers
		RendererState                              m_State{};
		DirectionalLight                           m_DirLight{};
		std::vector<Mesh*>                         m_UsedMeshes;
		std::vector<CommandBuffer>                 m_DrawList;
		std::array<InstanceData, max_objects>      m_InstancesData;
		std::array<PointLight, max_lights>         m_PointLights;
		std::array<SpotLight, max_lights>          m_SpotLights;
		std::array<glm::mat4, max_anim_joints>     m_AnimationJoints;
		std::unordered_map<Mesh*,InstancePackage>  m_Packages;
		std::unordered_map<Mesh*,uint32_t>         m_RootOffsets;

		struct PushConstant				           
		{								           
			glm::mat4                              DepthMVP = glm::mat4(1.0f);			                   
			uint32_t                               DataOffset = 0;
		};								           							                   
										           
		std::string                                m_Path = "";
		float                                      m_NearClip = 1.0f;
		float                                      m_FarClip = 1000.0f;
		glm::vec3                                  m_ShadowLightDirection = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::mat4                                  m_GridModel{};
		Frustum*                                   m_Frustum = nullptr;
		SceneData*                                 m_SceneData = nullptr;
		PushConstant                               m_MainPushConstant = {};
		// Sizes						           
		const size_t                               m_PushConstantSize = sizeof(PushConstant);
	};
}