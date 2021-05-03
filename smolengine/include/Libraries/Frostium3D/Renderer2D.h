#pragma once
#include "Common/RendererShared.h"

namespace Frostium
{
	struct Renderer2DStorage;
	class GraphicsPipeline;
	class Text;
	class Renderer2D
	{
	public:

		static void Init(Renderer2DStorage* storage);
		static void Shutdown();
		static void BeginScene(const ClearInfo* clearInfo, const BeginSceneInfo* info = nullptr);
		static void EndScene();

		// Submit
		static void SubmitSprite(const glm::vec2& worldPos, const glm::vec2& scale, const glm::vec2& rotation, uint32_t layerIndex, Texture* texture,
			const glm::vec4& color = glm::vec4(1.0f), GraphicsPipeline* material = nullptr);
		static void SubmitQuad(const glm::vec2& worldPos, const glm::vec2& scale, const glm::vec2& rotation, uint32_t layerIndex,
			const glm::vec4& color = glm::vec4(1.0f), GraphicsPipeline* material = nullptr);
		static void SubmitLight2D(const glm::vec2& worldPos, const glm::vec4& color, float radius, float lightIntensity);
		static void SubmitText(Text* text);

	private:

		static void Flush();
		static void StartNewBatch();
		static void Reset();

		static uint32_t AddTexture(Texture* tex);
		static void CreatePipelines();
		static void CreateFramebuffers();
		static void Prepare();
	private:

		inline static Renderer2DStats* Stats = nullptr;
	};
}