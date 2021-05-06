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
	static const uint32_t                  s_MaxInstances = 1000;
	static const uint32_t                  s_MaxAnimations = 100;
	static const uint32_t                  s_MaxAnimationJoints = 1000;
	static const uint32_t                  s_MaxPackages = 1200;
	static const uint32_t                  s_MaxDirectionalLights = 10;
	static const uint32_t                  s_MaxPointLights = 1000;
	static const uint32_t                  s_InstanceDataMaxCount = s_MaxPackages * s_MaxInstances;

	struct CommandBuffer
	{
		uint32_t                         InstancesCount = 0;
		uint32_t                         Offset = 0;
		Mesh*                            Mesh = nullptr;
	};

	struct DirectionalLightBuffer
	{
		glm::vec4                        Position;
		glm::vec4                        Color;
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
		bool                             m_IsInitialized = false;
		RendererState                    m_State{};
		ShadowMapSize                    m_MapSize = ShadowMapSize::SIZE_8;
		// Bindings
		const uint32_t                   m_TexturesBinding = 24;
		const uint32_t                   m_ShaderDataBinding = 25;
		const uint32_t                   m_MaterialsBinding = 26;
		const uint32_t                   m_SceneDataBinding = 27;
		const uint32_t                   m_AnimBinding = 28;
		const uint32_t                   m_DirLightBinding = 30;
		const uint32_t                   m_PointLightBinding = 31;
		const uint32_t                   m_AmbientLightBinding = 32;
		// Instance Data
		uint32_t                         m_Objects = 0;
		uint32_t                         m_InstanceDataIndex = 0;
		uint32_t                         m_UsedMeshesIndex = 0;
		uint32_t                         m_DrawListIndex = 0;
		uint32_t                         m_DirectionalLightIndex = 0;
		uint32_t                         m_PointLightIndex = 0;
		uint32_t                         m_LastAnimationOffset = 0;
		uint32_t                         m_MaxObjects = s_MaxPackages;
		// Pipelines
		GraphicsPipeline                 m_PBRPipeline = {};
		GraphicsPipeline                 m_BloomPipeline = {};
		GraphicsPipeline                 m_BlurPipeline = {};
		GraphicsPipeline                 m_CombinationPipeline = {};
		GraphicsPipeline                 m_SkyboxPipeline = {};
		GraphicsPipeline                 m_OmniPipeline = {};
		GraphicsPipeline                 m_DepthPassPipeline = {};
		GraphicsPipeline                 m_GridPipeline = {};
		GraphicsPipeline                 m_DebugPipeline = {};
		//Meshes
		Mesh                             m_PlaneMesh = {};
		// Framebuffers
		Framebuffer*                     m_MainFramebuffer = nullptr;
		Framebuffer                      m_PBRFramebuffer = {};
		Framebuffer                      m_BloomFramebuffer = {};
		Framebuffer                      m_BlurFramebuffer = {};
		Framebuffer                      m_DepthFramebuffer = {};
		// Buffers
		Mesh*                            m_UsedMeshes[s_MaxPackages];
		InstanceData                     m_InstancesData[s_InstanceDataMaxCount];
		CommandBuffer                    m_DrawList[s_MaxPackages];
		DirectionalLightBuffer           m_DirectionalLights[s_MaxDirectionalLights];
		PointLightBuffer                 m_PointLights[s_MaxPointLights];
		glm::mat4                        m_AnimationJoints[s_MaxAnimationJoints];
		std::unordered_map<Mesh*,
			InstancePackage>             m_Packages;
		std::unordered_map<Mesh*,
			uint32_t>                    m_RootOffsets;
		// UBO's & Push Constants
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
		struct ShadowMatrix
		{
			glm::mat4                    shadowTransforms[6];
		};

		std::string                      m_Path = "";
		float                            m_NearClip = 1.0f;
		float                            m_FarClip = 1000.0f;
		glm::vec3                        m_ShadowLightDirection = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::mat4                        m_GridModel{};
		Frustum*                         m_Frustum = nullptr;
		SceneData*                       m_SceneData = nullptr;
		PushConstant                     m_MainPushConstant = {};
		ShadowMatrix                     m_ShadowMatrix = {};
		AmbientLighting                  m_AmbientLighting = {};
		// Sizes
		const size_t                     m_PushConstantSize = sizeof(PushConstant);
		const size_t                     m_AmbientLightingSize = sizeof(AmbientLighting);
	};
}