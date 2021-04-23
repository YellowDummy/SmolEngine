#pragma once
#include "Core/Core.h"

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

		static void SubmitMeshes();
		static void SubmitLights();
		static void Submit2DTextures();
		static void Submit2DAnimations();
		static void SubmitCanvases(CameraComponent* camera, TransformComponent* cameraTransform);

		static void DebugDraw();
		static void CheckLayerIndex(int& index);

	private:

		friend class EditorLayer;
		friend class WorldAdmin;

	};
}