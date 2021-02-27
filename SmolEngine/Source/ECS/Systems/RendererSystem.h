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

		static void BeginDraw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3 camPos, float zNear, float zFar);

		static void EndDraw();

		// 3D

		static void SubmitMeshes(entt::registry& registry);

		static void SubmitDirectionalLights(entt::registry& registry);

		// 2D

		static void Submit2DTextures(entt::registry& registry);

		static void Submit2DLight(entt::registry& registry);

		static void Submit2DAnimations(entt::registry& registry);

		static void SubmitCanvases(entt::registry& registry, CameraComponent* camera, TransformComponent* cameraTransform);

		// Debug

		static void DebugDraw(entt::registry& registry);

		// Helpers

		static void CheckLayerIndex(int& index);

	private:

		friend class EditorLayer;
		friend class WorldAdmin;

	};
}