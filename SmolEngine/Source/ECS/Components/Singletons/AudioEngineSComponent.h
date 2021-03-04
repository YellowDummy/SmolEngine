#pragma once
#include "Core/Core.h"

#include "Audio/AudioEngine.h"

namespace SmolEngine
{
	// Note:
	// S - Singleton Component
	// Contains Audio Engine (FMOD) instance

	struct AudioEngineSComponent
	{
		AudioEngineSComponent();

		~AudioEngineSComponent();

		/// Data

		AudioEngine Engine = {};

		/// Getters

		static AudioEngineSComponent* Get() { return Instance; }

	private:

		static AudioEngineSComponent* Instance;
	};
}