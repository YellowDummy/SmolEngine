#pragma once

#include "Common/Events.h"
#include "InputCodes.h"

namespace Frostium 
{
	class Input
	{
	public:

		// Helpers
		static bool IsKeyPressed(KeyCode key);
		static bool IsMouseButtonPressed(MouseCode button);

		// Getters
		static float GetMouseX();
		static float GetMouseY();
		static std::pair<float, float> GetMousePosition();
	};

}