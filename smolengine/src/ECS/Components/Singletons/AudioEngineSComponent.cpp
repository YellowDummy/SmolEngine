#include "stdafx.h"
#include "ECS/Components/Singletons/AudioEngineSComponent.h"

#include "Audio/AudioEngine.h"

namespace SmolEngine
{
	AudioEngineSComponent* AudioEngineSComponent::Instance = nullptr;

	AudioEngineSComponent::AudioEngineSComponent()
	{
		Engine.Init();
		Instance = this;
	}

	AudioEngineSComponent::~AudioEngineSComponent()
	{
		if (!Instance) { return; }
		Instance = nullptr;
	}
}