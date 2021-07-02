#pragma once

#include "Common/Events.h"
#include "Common/InputCodes.h"

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
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