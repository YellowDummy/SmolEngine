#include "stdafx.h"
#include "ECS/Components/Singletons/JobsSystemStateSComponent.h"

namespace SmolEngine
{
	JobsSystemStateSComponent::JobsSystemStateSComponent()
	{
		NATIVE_INFO("ThreadPool -> avalable thareads: {}", Executor.num_workers());
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