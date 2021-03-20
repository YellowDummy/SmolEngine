#include "stdafx.h"
#include "JobsSystem.h"

#include <thread>
#include <algorithm>

namespace SmolEngine
{
	void JobsSystem::Complete(bool wait)
	{
		JobsSystemStateSComponent* instance = JobsSystemStateSComponent::GetSingleton();
		instance->Executor->run(*instance->Taskflow).wait();
	}

	void JobsSystem::Clear()
	{
		JobsSystemStateSComponent* instance = JobsSystemStateSComponent::GetSingleton();
		instance->Taskflow->clear();
	}

	void JobsSystem::BeginSubmition()
	{
		JobsSystemStateSComponent* instance = JobsSystemStateSComponent::GetSingleton();
		instance->bBeginSubmition = true;
		Clear();
	}

	void JobsSystem::EndSubmition(bool wait)
	{
		JobsSystemStateSComponent* instance = JobsSystemStateSComponent::GetSingleton();
		instance->bBeginSubmition = false;
		Complete(wait);
	}
}