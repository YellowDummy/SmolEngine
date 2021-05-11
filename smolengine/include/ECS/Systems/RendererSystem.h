#pragma once
#include "Core/Core.h"

#include <glm/glm.hpp>

namespace Frostium
{
	struct BeginSceneInfo;
	struct ClearInfo;
}

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

		static void BeginSubmit(Frostium::BeginSceneInfo* info);
		static void EndSubmit();
		static void Update();

		static void DebugDraw();
		static void CheckLayerIndex(int& index);

	private:

		inline static WorldAdminStateSComponent* m_World = nullptr;

		friend class EditorLayer;
		friend class WorldAdmin;

	};
}