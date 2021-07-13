#pragma once
#include "Core/Core.h"


namespace SmolEngine
{
	struct CameraComponent;
	struct TransformComponent;
	class Event;

	class CameraSystem
	{
		static void CalculateView(CameraComponent* camera, TransformComponent* transform);
		static void UpdateViewPerspective(CameraComponent* camera, TransformComponent* transform);
		static void UpdateViewOrtho(CameraComponent* camera, TransformComponent* transform);
		static void OnResize(uint32_t width, uint32_t height);

	private:
		friend class EditorLayer;
		friend class WorldAdmin;
		friend class GameView;
	};
}