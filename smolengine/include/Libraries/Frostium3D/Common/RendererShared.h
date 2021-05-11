#pragma once

#include "Common/Core.h"
#include "Common/Texture.h"
#include "Common/SubTexture.h"
#include "Common/BufferLayout.h"

#include "EditorCamera.h"
#include "Utils/GLM.h"

#include <glm/glm.hpp>

namespace Frostium
{
	struct RendererStorage;
	struct Renderer2DStorage;

	class EditorCamera;
	class Frustum;

	enum class DebugPrimitives : uint16_t
	{
		None = 0,
		Quad,
		Circle
	};

	enum class ShadowMapSize : uint16_t
	{
		SIZE_2,
		SIZE_4,
		SIZE_8,
		SIZE_16
	};

	struct BeginSceneInfo
	{
		void Update(Camera* cam)
		{
			View = cam->GetViewMatrix();
			Proj = cam->GetProjection();
			Pos = cam->GetPosition();
			NearClip = cam->GetNearClip();
			FarClip = cam->GetFarClip();
		}

	public:

		float                              NearClip = 0.0f;
		float                              FarClip = 0.0f;
		float                              Exoposure = 0.0f;
					                       
		glm::vec3                          Pos = glm::vec3(0.0f);
		glm::mat4                          Proj = glm::mat4(1.0f);
		glm::mat4                          View = glm::mat4(1.0f);
	};

	struct ClearInfo
	{
		bool                               bClear = true;
		glm::vec4                          color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	};

	struct SceneData
	{
		alignas(4) float                   NearClip = 0.0f;
		alignas(4) float                   FarClip = 0.0f;
		alignas(4) float                   Exoposure = 1.0f;
		alignas(4) float                   Pad1 = 0.0f;

		alignas(16) glm::mat4              Projection = glm::mat4(1.0f);
		alignas(16) glm::mat4              View = glm::mat4(1.0f);
		alignas(16) glm::mat4              SkyBoxMatrix = glm::mat4(1.0f);
		alignas(16) glm::vec4              CamPos = glm::vec4(1.0f);
		alignas(16) glm::vec4              AmbientColor = glm::vec4(1.0f);
	};

	struct FullscreenVertex
	{
		glm::vec2              pos;
		glm::vec2              uv;
	};

	struct Renderer2DStats
	{
		uint32_t               DrawCalls = 0;
		uint32_t               QuadCount = 0;
		uint32_t               TexturesInUse = 0;
		uint32_t               LayersInUse = 0;

		void Reset()
		{
			DrawCalls = 0;
			QuadCount = 0;
			TexturesInUse = 0;
			LayersInUse = 0;
		}

		inline uint32_t GetTotalVertexCount() { return QuadCount * 4; }
		inline uint32_t GetTotalIndexCount() { return QuadCount * 6; }
	};
}