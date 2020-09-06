#pragma once
#include "Core/Core.h"
#include "Core/Renderer/Texture.h"
#include "Core/Renderer/SubTexture.h"
#include <glm/glm.hpp>


namespace SmolEngine
{
	class OrthographicCamera;
	enum class DebugPrimitives
	{
		None = 0,
		Quad, Circle
	};

	//TODO: Implement Batch Rendering
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(Ref<OrthographicCamera> camera, float ambientValue);
		static void EndScene();

		static void DrawQuadRotated(const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const glm::vec4& color);
		static void DrawSprite(const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const Ref<Texture2D>& texture,
float repeatValue = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawQuad(const glm::vec3& worldPos, const glm::vec2& scale, const glm::vec4& color);

		//Debug
		static void BeginDebug(Ref<OrthographicCamera> camera);
		static void EndDebug();
		static void DebugDraw(DebugPrimitives type, const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const glm::vec4& color = glm::vec4(0.121f, 1.0f, 0.058f, 1.0f));

		//Light
		static void DrawLight2D(const glm::vec3& worldPos, const glm::vec2& scale, const glm::vec4& color, const float lightIntensity, Ref<OrthographicCamera> camera);

		struct RendererData2D
		{
			uint32_t DrawCalls;
			uint32_t QuadCount;

			inline uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			inline uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};
	};
}