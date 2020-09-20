#pragma once
#include "Core/Core.h"

#include <string>

namespace SmolEngine
{
	class Scene;
	class Animation2D;
	class CameraController;

	class SettingsWindow
	{
	public:

		//TODO: Add ImGui Flags
		SettingsWindow() = default;
		void Update(bool& isOpened, Ref<Scene> scene);
	};

	class ActorCreationWindow
	{
	public:

		ActorCreationWindow() = default;
		void Update(bool& isOpened, Ref<Scene> scene);
	};

}