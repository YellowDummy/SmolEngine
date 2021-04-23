#pragma once
#include "Core/Core.h"


namespace SmolEngine
{
	struct CameraComponent;
	struct TransformComponent;
	class Event;

	class CameraSystem
	{
	public:

		CameraSystem();

		static void CalculateView(CameraComponent* camera, TransformComponent* tranform);
		static void SetProjection(CameraComponent* camera, float left, float right, float buttom, float top, float zNear = -1.0f, float zFar = 1.0f);

	private:

		static void OnResize(float width, float height);
		static void OnEvent(Event& e);

	private:

		friend class EditorLayer;
		friend class WorldAdmin;
	};
}