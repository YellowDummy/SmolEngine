#pragma once

#include "GraphicsPipeline.h"

#include "Common/Framebuffer.h"
#include "Common/Texture.h"
#include "Common/Mesh.h"
#include "Common/RendererShared.h"

#include "Utils/Frustum.h"
#include "Utils/GLM.h"

namespace Frostium
{
	static const uint32_t                  s_MaxInstances = 1000;
	static const uint32_t                  s_MaxPackages = 1200;
	static const uint32_t                  s_MaxDirectionalLights = 1000;
	static const uint32_t                  s_MaxPointLights = 1000;
	static const uint32_t                  s_InstanceDataMaxCount = s_MaxPackages * s_MaxInstances;

	struct CommandBuffer
	{
		uint32_t                         InstancesCount = 0;
		uint32_t                         Offset = 0;
		Mesh* Mesh = nullptr;
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

	struct RendererStorage
	{
		RendererStorage() { m_Packages.reserve(s_MaxPackages); }
		~RendererStorage() = default;

		// States
		bool                             m_IsInitialized = false;
		bool                             m_ShowDebugView = false;
		bool                             m_IsBloomPassActive = true;
		bool                             m_IsBlurPassActive = false;
		ShadowMapSize                    m_MapSize = ShadowMapSize::SIZE_8;
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
		GraphicsPipeline                 m_PBRPipeline = {};
		GraphicsPipeline                 m_BloomPipeline = {};
		GraphicsPipeline                 m_BlurPipeline = {};
		GraphicsPipeline                 m_CombinationPipeline = {};
		GraphicsPipeline                 m_SkyboxPipeline = {};
		GraphicsPipeline                 m_OmniPipeline = {};
		GraphicsPipeline                 m_DepthPassPipeline = {};
		GraphicsPipeline                 m_DebugPipeline = {};
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
		std::unordered_map<Mesh*,
			InstancePackage>             m_Packages;
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
		struct DebugView
		{
			uint32_t                     ShowOmniCube = 0;
			uint32_t                     ShowMRT = 0;
			uint32_t                     MRTattachmentIndex = 0;
		};
		struct ShadowMatrix
		{
			glm::mat4                    shadowTransforms[6];
		};

		std::string                      m_Path = "";
		float                            m_NearClip = 1.0f;
		float                            m_FarClip = 1000.0f;
		glm::vec3                        m_ShadowLightDirection = glm::vec3(0.0f, 0.0f, 0.0f);
		Frustum* m_Frustum = nullptr;
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
}