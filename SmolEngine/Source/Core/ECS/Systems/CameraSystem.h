#pragma once
#include "Core/Core.h"

#include <entt.hpp>

namespace SmolEngine
{
	struct CameraComponent;

	struct TransformComponent;

	class Event;

	class CameraSystem
	{
	public:

		CameraSystem();

		///

		static void CalculateView(CameraComponent* camera, TransformComponent* tranform);

		static void SetProjection(CameraComponent* camera, float left, float right, float buttom, float top, float zNear = -1.0f, float zFar = 1.0f);

	private:

		static void OnResize(entt::registry& registry, float width, float height);

		static void OnEvent(entt::registry& registry, Event& e);

	private:

		friend class EditorLayer;
		friend class WorldAdmin;
	};
}