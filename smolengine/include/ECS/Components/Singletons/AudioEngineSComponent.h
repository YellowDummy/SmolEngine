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

		AudioEngine Engine = {};
		static AudioEngineSComponent* Get() { return Instance; }

	private:

		static AudioEngineSComponent* Instance;
	};
}