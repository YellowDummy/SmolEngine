#pragma once

#include "Core/Renderer/Renderer2DData.h"

/// 07.11.2020
/// 
/// 2D Batch Renderer

namespace SmolEngine
{
	class OrthographicCamera;

	class Framebuffer;

	///

	class Renderer2D
	{
	public:

		static void Init();

		static void Shutdown();

		static void BeginScene(const glm::mat4& viewProjectionMatrix, const float ambientValue, Ref<Framebuffer> targetFramebuffer);

		static void EndScene();

		// Batch
		
		static void DrawLayer(LayerDataBuffer& layer);

		static void FlushLayer(LayerDataBuffer& layer);

		static void ResetLayer(LayerDataBuffer& layer);

		static void StartNewBatch();

		static void FlushAllLayers();

		static void UploadLightUniforms();

		static void ClearBuffers();

		// Submit

		static void SubmitSprite(const glm::vec3& worldPos, const uint32_t layerIndex, const glm::vec2& scale, const float rotation, const Ref<Texture2D>& texture,
			const float repeatValue = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void SubmitQuad(const glm::vec3& worldPos, const uint32_t layerIndex, const glm::vec2& scale, const float rotation, const glm::vec4& color);

		static void DrawUIText(const glm::vec3& pos, const glm::vec2& scale, const Ref<Texture2D> texture, const glm::vec4& tintColor = glm::vec4(1.0f));

		// Debug

		static void BeginDebug();

		static void EndDebug();

		static void DebugDrawQuad(const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const glm::vec4& color = glm::vec4(0.121f, 1.0f, 0.058f, 1.0f));

		static void DebugDrawCircle(const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const glm::vec4& color = glm::vec4(0.121f, 1.0f, 0.058f, 1.0f));

		static void DebugDrawLine(const glm::vec3& startPos, const glm::vec3& endPos, const glm::vec4& color = glm::vec4(0.121f, 1.0f, 0.058f, 1.0f));

		// Light

		static void SubmitLight2D(const glm::vec3& offset, const float radius, const glm::vec4& color, const float lightIntensity);

		// Animation

		static void DrawAnimation2DPreview(Ref<OrthographicCamera> camera, float ambientValue, const glm::vec3& worldPos, const glm::vec2& scale, const float rotation, const Ref<Texture2D>& texture,
			float repeatValue = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		// Framebuffer

		static void DrawFrameBuffer(const uint32_t colorAttachmentID);

		// Helpers

		static void CreateBatchData();

		static void CreateDebugData();

		static void CreateFramebufferData();

	public:

		static const uint32_t MaxTextureSlot = 32; // TODO: get TMU (Texture Mapping Units) at initialization

	private:

		static bool FindShader(std::string& filePath, const std::string& shaderName);

	private:

		static Renderer2DStats* Stats;
		inline static bool m_RenderResetted = false;

	private:

		friend class WorldAdmin;

		friend class EditorLayer;
	};
}