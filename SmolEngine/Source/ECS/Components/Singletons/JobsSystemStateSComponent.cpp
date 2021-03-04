#include "stdafx.h"
#include "JobsSystemStateSComponent.h"

namespace SmolEngine
{
	JobsSystemStateSComponent::JobsSystemStateSComponent()
	{
		Instance = this;
	}

	JobsSystemStateSComponent::~JobsSystemStateSComponent()
	{
		Instance = nullptr;
	}

	JobsSystemStateSComponent* JobsSystemStateSComponent::GetSingleton()
	{
		return Instance;
	}
}