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
	class EditorCamera;
	enum class DebugPrimitives : uint16_t
	{
		None = 0,
		Quad,
		Circle
	};

	struct BeginSceneInfo
	{
		void Update(EditorCamera* cam)
		{
			view = cam->GetViewMatrix();
			proj = cam->GetProjection();
			pos = cam->GetPosition();
			nearClip = cam->GetNearClip();
			farClip = cam->GetFarClip();
		}

	public:

		float          nearClip;
		float          farClip;

		glm::vec3      pos;
		glm::mat4      proj;
		glm::mat4      view;
	};

	struct ClearInfo
	{
		bool      bClear = false;
		glm::vec4 color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	};

	struct SceneData
	{
		glm::mat4                    Projection = glm::mat4(1.0f);
		glm::mat4                    View = glm::mat4(1.0f);
		glm::mat4                    SkyBoxMatrix = glm::mat4(1.0f);
		glm::vec4                    CamPos = glm::vec4(1.0f);

		glm::vec4                    Params = glm::vec4(2.0f, 4.0f, 1.0f, 1.0f);
	};

	struct DirectionalLightBuffer
	{
		glm::vec4 Position;
		glm::vec4 Color;
	};

	struct PointLightBuffer
	{
		glm::vec4 Position;
		glm::vec4 Color;
		glm::vec4 Params; // x = Constant, y = Linear, z = Exp
	};

	struct FullscreenVertex
	{
		glm::vec2 pos;
		glm::vec2 uv;
	};

	struct Renderer2DStats
	{
		uint32_t DrawCalls = 0;
		uint32_t QuadCount = 0;
		uint32_t TexturesInUse = 0;
		uint32_t LayersInUse = 0;

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