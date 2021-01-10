#pragma once
#include "Core/Core.h"

#include <entt.hpp>
#include <glm/glm.hpp>

namespace SmolEngine
{
	struct Animation2DComponent;
	struct Body2DComponent;
	struct CanvasComponent;
	struct TransformComponent;
	struct CameraComponent;
	struct Texture2DComponent;
	class Framebuffer;

	class RendererSystem
	{
	public:

		RendererSystem() = default;

		static void SubmitDebugQuad(const glm::vec3& worldPos, const glm::vec2 scale, float rotation,
			const glm::vec4& color = glm::vec4(0.121f, 1.0f, 0.058f, 1.0f));

		static void SubmitDebugCircle(const glm::vec3& worldPos, float radius, float rotation,
			const glm::vec4& color = glm::vec4(0.121f, 1.0f, 0.058f, 1.0f));

		static void SubmitDebugLine(const glm::vec3& startPos, const glm::vec3& endPos,
			const glm::vec4& color = glm::vec4(0.121f, 1.0f, 0.058f, 1.0f));

	private:

		static void BeginDraw(const glm::mat4& viewProjectionMatrix, const float ambientValue, Ref<Framebuffer>& targetFramebuffer);

		static void EndDraw();

		// Textures

		static void Render2DTextures(entt::registry& registry);

		// Light 2D

		static void Render2DLight(entt::registry& registry);

		// Animation

		static void Render2DAnimations(entt::registry& registry);

		// UI

		static void RenderCanvases(entt::registry& registry, CameraComponent* camera, TransformComponent* cameraTransform);

		// Debug

		static void DebugDraw(entt::registry& registry);

		// Helpers

		static void CheckLayerIndex(int& index);

	private:

		friend class EditorLayer;
		friend class WorldAdmin;

	};
}