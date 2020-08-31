#pragma once
#include "Core/Core.h"

namespace SmolEngine
{
	class Scene;

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