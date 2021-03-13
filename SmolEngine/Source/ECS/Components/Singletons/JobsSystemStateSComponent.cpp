#include "stdafx.h"
#include "JobsSystemStateSComponent.h"

namespace SmolEngine
{
	JobsSystemStateSComponent::JobsSystemStateSComponent()
	{
		ThreadPoolInstance = new ThreadPool();
		Instance = this;
	}

	JobsSystemStateSComponent::~JobsSystemStateSComponent()
	{
		delete ThreadPoolInstance;
		Instance = nullptr;
	}

	JobsSystemStateSComponent* JobsSystemStateSComponent::GetSingleton()
	{
		return Instance;
	}
}