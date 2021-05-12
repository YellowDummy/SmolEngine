#pragma once
#include "Common/Core.h"
#include "Common/RendererShared.h"

#include "Utils/GLM.h"

namespace Frostium
{
	class Mesh;

	class DebugRenderer
	{
	public:

		static void BeginDebug();
		static void EndDebug();

		static void DrawLine(const glm::vec3& pos1, const glm::vec3& pos2, float width = 1.0f, const glm::vec4& color = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& pos, const glm::vec3& rotation, const glm::vec3& scale);
		static void DrawWireframes(const glm::vec3& pos, const glm::vec3& rotation, const glm::vec3& scale, Mesh* mesh);
		static void DrawCirlce(const glm::vec3& pos, const glm::vec3& scale);


	private:

		static void Init();

		friend class GraphicsContext;
		friend class Renderer;
	};
}