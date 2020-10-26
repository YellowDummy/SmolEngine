#include "stdafx.h"
#include "AudioEngineSComponent.h"

#include "Core/Audio/AudioEngine.h"

namespace SmolEngine
{
	AudioEngineSComponent* AudioEngineSComponent::Instance = nullptr;

	AudioEngineSComponent::AudioEngineSComponent()
	{
		Engine = new AudioEngine();
		Engine->Init();

		Instance = this;
	}

	AudioEngineSComponent::~AudioEngineSComponent()
	{
		if (!Instance) { return; }

		if (Engine != nullptr)
		{
			delete Engine;
		}

		Instance = nullptr;
	}
}