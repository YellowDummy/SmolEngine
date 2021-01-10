#pragma once
#include "Core/Core.h"

namespace SmolEngine
{
	class AudioEngine;

	// Note:
	// S - Singleton Component
	// Contains Audio Engine (FMOD) instance

	struct AudioEngineSComponent
	{
		AudioEngineSComponent();

		~AudioEngineSComponent();

		/// Data

		AudioEngine* Engine = nullptr;

		/// Getters

		static AudioEngineSComponent* Get() { return Instance; }

	private:

		static AudioEngineSComponent* Instance;
	};
}