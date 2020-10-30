#pragma once
#include "Core/Core.h"


namespace SmolEngine
{
	class WorldAdmin;

	class Animation2D;

	class CameraController;

	class SettingsWindow
	{
	public:

		SettingsWindow() = default;

		void Update(bool& isOpened, Ref<WorldAdmin> scene);
	};

	class ActorCreationWindow
	{
	public:

		ActorCreationWindow() = default;

		void Update(bool& isOpened, Ref<WorldAdmin> scene);
	};

}