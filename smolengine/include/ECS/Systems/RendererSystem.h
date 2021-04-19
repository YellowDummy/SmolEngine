#pragma once
#include "Core/Core.h"

#include <entt/entt.hpp>
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
	private:

		static void BeginDraw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3 camPos, float zNear, float zFar);
		static void EndDraw();

		static void SubmitMeshes(entt::registry& registry);
		static void SubmitLights(entt::registry& registry);
		static void Submit2DTextures(entt::registry& registry);
		static void Submit2DAnimations(entt::registry& registry);
		static void SubmitCanvases(entt::registry& registry, CameraComponent* camera, TransformComponent* cameraTransform);

		static void DebugDraw(entt::registry& registry);
		static void CheckLayerIndex(int& index);

	private:

		friend class EditorLayer;
		friend class WorldAdmin;

	};
}