#pragma once

#include "Core/Core.h"

namespace SmolEngine
{
	class EngineCommand
	{
	public:

		EngineCommand() = default;

		/// Main

		static void LoadScene(uint32_t index);

		static void CloseApp();
	};
}