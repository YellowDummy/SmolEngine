#pragma once
#include "Core/Renderer/Texture.h"
#include "Core/Renderer/SubTexture.h"
#include <glm/glm.hpp>


namespace SmolEngine
{
	class OrthographicCamera;

	class Renderer2D
	{
	public:
		static void Init();
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void Flush();

		static void DrawQuadRotated(const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const glm::vec4& color);
		static void DrawSprite(const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const Ref<Texture2D>& texture, float repeatValue = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawTexture(const glm::mat4& transform, const glm::vec4& color);
		static void DrawTexture(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f), float repeatValue = 1.0f);

		static void DrawQuad(const glm::vec3& worldPos, const glm::vec2& scale, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& worldPos, const glm::vec2& scale, const Ref<Texture2D>& texture, float repeatValue = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawSpriteSheetTexture(const glm::vec3& worldPos, const glm::vec2& scale, const Ref<SubTexture2D>& subTexture, float repeatValue = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void Shutdown();

		struct RendererData2D
		{
			uint32_t DrawCalls;
			uint32_t QuadCount;

			inline uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			inline uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};

		static void ResetDataStats();
		static RendererData2D& GetData();
		static void StartNewBath();
	};
}