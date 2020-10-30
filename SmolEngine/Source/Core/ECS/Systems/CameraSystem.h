#pragma once
#include "Core/Core.h"

namespace SmolEngine
{
	struct CameraBaseTuple;

	class Event;

	///

	class CameraSystem
	{
	public:

		CameraSystem();

		///

		static void CalculateView(CameraBaseTuple& tuple);

		static void SetProjection(CameraBaseTuple& tuple, float left, float right, float buttom, float top, float zNear = -1.0f, float zFar = 1.0f);

	private:

		static void OnResize(CameraBaseTuple& tuple, float width, float height);

		static void OnEvent(CameraBaseTuple& tuple, Event& e);

		///

	private:

		friend class EditorLayer;

		friend class WorldAdmin;
	};
}