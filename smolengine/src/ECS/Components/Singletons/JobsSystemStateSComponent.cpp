#include "stdafx.h"
#include "ECS/Components/Singletons/JobsSystemStateSComponent.h"

namespace SmolEngine
{
	JobsSystemStateSComponent::JobsSystemStateSComponent()
	{
		Instance = this;
	}

	JobsSystemStateSComponent::~JobsSystemStateSComponent()
	{

	}

	JobsSystemStateSComponent* JobsSystemStateSComponent::GetSingleton()
	{
		return Instance;
	}
}