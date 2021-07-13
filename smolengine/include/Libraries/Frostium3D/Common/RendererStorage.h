#pragma once

#include "GraphicsPipeline.h"

#include "Primitives/Framebuffer.h"
#include "Primitives/Texture.h"
#include "Primitives/Mesh.h"

#include "Common/EnvironmentMap.h"
#include "Common/RendererShared.h"
#include "Common/Animation.h"

#include "Utils/Frustum.h"
#include "Utils/GLM.h"

#include <mutex>

class VulkanPBR;

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
#pragma region Limits
	static const uint32_t      max_animations = 100;
	static const uint32_t      max_anim_joints = 1000;
	static const uint32_t      max_materials = 1000;
	static const uint32_t      max_lights = 1000;
	static const uint32_t      max_objects = 15000;
#pragma endregion

#pragma region Shader Defs

	struct DirectionalLight
	{
		glm::vec4  Direction = glm::vec4(67, 56, 0, 0);
		glm::vec4  Color = glm::vec4(1.0);
		float      Intensity = 1.0f;
		float      Bias = 1.0f;
		float      zNear = 1.0f;
		float      zFar = 350.0f;
		float      lightFOV = 45.0f;
		uint32_t   IsActive = false;
		uint32_t   IsCastShadows = false;
		uint32_t   IsUseSoftShadows = true;
	};

	struct PointLight
	{
		glm::vec4  Position = glm::vec4(0);
		glm::vec4  Color = glm::vec4(1.0);
		float      Intensity = 1.0f;
		float      Raduis = 10.0f;
		float      Bias = 1.0f;
		uint32_t   IsActive = true;
		uint32_t   IsCastShadows = false;
	private:
		uint32_t   pad1 = 0;
		uint32_t   pad2 = 0;
		uint32_t   pad3 = 0;
	};

	struct SpotLight
	{
		glm::vec4  Position = glm::vec4(0, 0, 0, 0);
		glm::vec4  Direction = glm::vec4(0, 0, 40, 0);
		glm::vec4  Color = glm::vec4(1.0);
		float      Intensity = 10.0f;
		float      CutOff = 40.0f;
		float      OuterCutOff = 5.0f;
		float      Raduis = 10.0f;
		float      Bias = 0.005f;
		uint32_t   IsActive = true;
		uint32_t   IsCastShadows = false;
	private:
		uint32_t pad1 = 0;
	};

	struct InstanceData
	{
		uint32_t       MaterialID = 0;
		uint32_t       IsAnimated = false;
		uint32_t       AnimOffset = 0;
		uint32_t       EntityID = 0;
		glm::mat4      ModelView = glm::mat4(1.0f);
	};

	struct LightingProperties
	{
		glm::vec4   AmbientColor = glm::vec4(1.0f);
		float       IBLStrength = 1.0f;
	private:
		uint32_t    UseIBL = 1;
		uint32_t    pad1 = 1;
		uint32_t    pad2 = 1;

		friend class DeferredRenderer;
	};

	struct BloomProperties
	{
		float Exposure = 1.0f;
		float Threshold = 0.10f;
		float Scale = 0.5f;
		float Strength = 0.5f;
	};

	struct FXAAProperties
	{
		float      EdgeThresholdMin = 0.0312f;
		float      EdgeThresholdMax = 0.125f;
		float      Iterations = 29.0f;
		float      SubPixelQuality = 0.75f;
		// don't use! for internal needs
		glm::vec2  InverseScreenSize = glm::vec2(0.0f);
	private:
		float      Pad1 = 1.0f;
		float      Pad2 = 1.0f;

	};

#pragma endregion

#pragma region Mask

	struct DirtMask
	{
		Texture* Mask = nullptr;
		float    Intensity = 1.0f;
		float    BaseIntensity = 0.1f;
	};
#pragma endregion

#pragma region Srotage

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
		bool                   bFXAA = true;
		bool                   bBloom = false;
		bool                   bIBL = true;
		float                  FrustumRadius = 20.0f;
		DebugViewFlags         eDebugView = DebugViewFlags::None;
		LightingProperties     Lighting = {};
		BloomProperties        Bloom = {};
		FXAAProperties         FXAA = {};
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
		const uint32_t                             m_LightingStateBinding = 33;
		const uint32_t                             m_BloomStateBinding = 34;
		const uint32_t                             m_FXAAStateBinding = 35;
		const uint32_t                             m_DynamicSkyBinding = 36;
		// Instance Data				           
		uint32_t                                   m_Objects = 0;
		uint32_t                                   m_InstanceDataIndex = 0;
		uint32_t                                   m_PointLightIndex = 0;
		uint32_t                                   m_SpotLightIndex = 0;
		uint32_t                                   m_LastAnimationOffset = 0;
		// Pipelines					           
		GraphicsPipeline                           p_Gbuffer = {};
		GraphicsPipeline                           p_Lighting = {};
		GraphicsPipeline                           p_Bloom = {};
		GraphicsPipeline                           p_Combination = {};
		GraphicsPipeline                           p_Skybox = {};
		GraphicsPipeline                           p_DepthPass = {};
		GraphicsPipeline                           p_Grid = {};
		GraphicsPipeline                           p_Debug = {};
		GraphicsPipeline                           p_Mask = {};
		// Framebuffers	
		Framebuffer*                               f_Main = nullptr;
		Framebuffer                                f_GBuffer= {};
		Framebuffer                                f_Lighting = {};
		Framebuffer                                f_Bloom = {};
		Framebuffer                                f_Depth = {};
		// Masks
		DirtMask                                   m_DirtMask = {};
		//Meshes						           
		Mesh                                       m_PlaneMesh = {};
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
		PushConstant                               m_MainPushConstant = {};
		Frustum*                                   m_Frustum = nullptr;
		SceneData*                                 m_SceneData = nullptr;
		VulkanPBR*                                 m_VulkanPBR = nullptr;
		Ref<EnvironmentMap>                        m_EnvironmentMap = nullptr;
		// Sizes						           
		const size_t                               m_PushConstantSize = sizeof(PushConstant);
	};
}