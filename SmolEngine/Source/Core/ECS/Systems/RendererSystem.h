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

	///

	class RendererSystem
	{
	public:

		RendererSystem() = default;


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

		static void DebugDraw(const Body2DComponent& body2D, const TransformComponent& transform);

	private:

		// Helpers

		static void CheckLayerIndex(int& index);

	private:

		friend class EditorLayer;
		friend class WorldAdmin;

	};
}