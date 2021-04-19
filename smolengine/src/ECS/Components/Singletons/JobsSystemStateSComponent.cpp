#include "stdafx.h"
#include "ECS/Components/Singletons/JobsSystemStateSComponent.h"

namespace SmolEngine
{
	JobsSystemStateSComponent::JobsSystemStateSComponent()
	{
		Taskflow = new tf::Taskflow();
		Executor = new tf::Executor();

		NATIVE_INFO("ThreadPool -> avalable thareads: {}", Executor->num_workers());
		Instance = this;
	}

	JobsSystemStateSComponent::~JobsSystemStateSComponent()
	{
		delete Taskflow, Executor;
		Instance = nullptr;
	}

	JobsSystemStateSComponent* JobsSystemStateSComponent::GetSingleton()
	{
		return Instance;
	}
}