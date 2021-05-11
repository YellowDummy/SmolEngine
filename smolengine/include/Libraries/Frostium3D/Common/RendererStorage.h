#pragma once

#include "GraphicsPipeline.h"

#include "Common/Framebuffer.h"
#include "Common/Texture.h"
#include "Common/Mesh.h"
#include "Common/RendererShared.h"
#include "Common/Animation.h"

#include "Utils/Frustum.h"
#include "Utils/GLM.h"

namespace Frostium
{
	static const uint32_t  s_MaxInstances = 1000;
	static const uint32_t  s_MaxAnimations = 100;
	static const uint32_t  s_MaxAnimationJoints = 1000;
	static const uint32_t  s_MaxPackages = 1200;
	static const uint32_t  s_MaxLights = 100;
	static const uint32_t  s_InstanceDataMaxCount = s_MaxPackages * s_MaxInstances;

	struct DepthPassProperties
	{
		float                  zNear = 1.0f;
		float                  zFar = 500.0f;
		float                  lightFOV = 45.0f;
		glm::vec3              lightPos = glm::vec3(20, 60, 0);
	};

	struct SceneState
	{
		alignas(4) float       HDRExposure = 1.0f;
		alignas(4) uint32_t    UseIBL = true;
	private:
		alignas(4) uint32_t    NumPointsLights = 0;
		alignas(4) uint32_t    NumSpotLights = 0;

		friend class Renderer;
	};

	struct RendererState
	{
		bool                   bBloomPass = true;
		bool                   bBlurPass = false;
		bool                   bDrawSkyBox = true;
		bool                   bDrawGrid = true;       
		SceneState*            pSceneState = nullptr;
	};

	struct CommandBuffer
	{
		uint32_t               InstancesCount = 0;
		uint32_t               Offset = 0;
		Mesh*                  Mesh = nullptr;
	};

	struct DirectionalLight
	{
		alignas(16) glm::vec4  Direction = glm::vec4(67, 56, 0, 0);
		alignas(16) glm::vec4  Color = glm::vec4(1.0);
							   
		alignas(4) float       Intensity = 1.0f;
		alignas(4) float       Bias = 1.0f;
		alignas(4) uint32_t    IsActive = false;
		alignas(4) uint32_t    IsCastShadows = false;
		alignas(4) uint32_t    IsUseSoftShadows = true;

	private:
		alignas(4) uint32_t    pad1 = 0;
		alignas(4) uint32_t    pad2 = 0;
		alignas(4) uint32_t    pad3 = 0;
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

	struct InstancePackage
	{
		struct Package
		{
			uint32_t                     MaterialID = 0;
			glm::vec3*                   WorldPos = nullptr;
			glm::vec3*                   Rotation = nullptr;
			glm::vec3*                   Scale = nullptr;
		};

		uint32_t                         CurrentIndex = 0;
		Package                          Data[s_MaxInstances];
	};

	struct RendererStorage
	{
		RendererStorage() { m_Packages.reserve(s_MaxPackages); }
		~RendererStorage() = default;

		// States
		bool                                               m_IsInitialized = false;
		RendererState                                      m_State{};
		ShadowMapSize                                      m_MapSize = ShadowMapSize::SIZE_8;
		// Bindings						                   
		const uint32_t                                     m_TexturesBinding = 24;
		const uint32_t                                     m_ShaderDataBinding = 25;
		const uint32_t                                     m_MaterialsBinding = 26;
		const uint32_t                                     m_SceneDataBinding = 27;
		const uint32_t                                     m_AnimBinding = 28;
		const uint32_t                                     m_PointLightBinding = 30;
		const uint32_t                                     m_SpotLightBinding = 31;
		const uint32_t                                     m_DirLightBinding = 32;
		const uint32_t                                     m_SceneStateBinding = 33;
		// Instance Data				                   
		uint32_t                                           m_Objects = 0;
		uint32_t                                           m_InstanceDataIndex = 0;
		uint32_t                                           m_UsedMeshesIndex = 0;
		uint32_t                                           m_DrawListIndex = 0;
		uint32_t                                           m_PointLightIndex = 0;
		uint32_t                                           m_SpotLightIndex = 0;
		uint32_t                                           m_LastAnimationOffset = 0;
		uint32_t                                           m_MaxObjects = s_MaxPackages;
		// Pipelines					                   
		GraphicsPipeline                                   m_PBRPipeline = {};
		GraphicsPipeline                                   m_BloomPipeline = {};
		GraphicsPipeline                                   m_BlurPipeline = {};
		GraphicsPipeline                                   m_CombinationPipeline = {};
		GraphicsPipeline                                   m_SkyboxPipeline = {};
		GraphicsPipeline                                   m_OmniPipeline = {};
		GraphicsPipeline                                   m_DepthPassPipeline = {};
		GraphicsPipeline                                   m_GridPipeline = {};
		GraphicsPipeline                                   m_DebugPipeline = {};
		//Meshes						                   
		Mesh                                               m_PlaneMesh = {};
		// Framebuffers					                   
		Framebuffer*                                       m_MainFramebuffer = nullptr;
		Framebuffer                                        m_PBRFramebuffer = {};
		Framebuffer                                        m_BloomFramebuffer = {};
		Framebuffer                                        m_BlurFramebuffer = {};
		Framebuffer                                        m_DepthFramebuffer = {};
		// Buffers
		SceneState                                         m_SceneState{};
		DirectionalLight                                   m_DirLight{};
		DepthPassProperties                                m_DepthPassProperties{};
		std::array<Mesh*, s_MaxPackages>                   m_UsedMeshes;
		std::array<InstanceData, s_InstanceDataMaxCount>   m_InstancesData;
		std::array<CommandBuffer, s_MaxPackages>           m_DrawList;
		std::array<PointLight,s_MaxLights>                 m_PointLights;
		std::array<SpotLight, s_MaxLights>                 m_SpotLights;
		std::array<glm::mat4, s_MaxAnimationJoints>        m_AnimationJoints;
		std::unordered_map<Mesh*,InstancePackage>          m_Packages;
		std::unordered_map<Mesh*,uint32_t>                 m_RootOffsets;

		struct PushConstant				                   
		{								                   
			glm::mat4                                      DepthMVP = glm::mat4(1.0f);			                   
			uint32_t                                       DataOffset = 0;
		};								                   							                   
										                   
		std::string                                        m_Path = "";
		float                                              m_NearClip = 1.0f;
		float                                              m_FarClip = 1000.0f;
		glm::vec3                                          m_ShadowLightDirection = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::mat4                                          m_GridModel{};
		Frustum*                                           m_Frustum = nullptr;
		SceneData*                                         m_SceneData = nullptr;
		PushConstant                                       m_MainPushConstant = {};
		// Sizes						                   
		const size_t                                       m_PushConstantSize = sizeof(PushConstant);
	};
}