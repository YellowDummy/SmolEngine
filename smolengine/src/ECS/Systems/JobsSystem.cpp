#include "stdafx.h"
#include "ECS/Systems/JobsSystem.h"

namespace SmolEngine
{
	void JobsSystem::Complete(bool wait)
	{
		JobsSystemStateSComponent* instance = JobsSystemStateSComponent::GetSingleton();
		instance->Executor.run(instance->Taskflow).wait();
	}

	void JobsSystem::Clear()
	{
		JobsSystemStateSComponent* instance = JobsSystemStateSComponent::GetSingleton();
		instance->Taskflow.clear();
	}

	void JobsSystem::BeginSubmition()
	{
		JobsSystemStateSComponent* instance = m_State;
		instance->bBeginSubmition = true;
		Clear();
	}

	void JobsSystem::EndSubmition(bool wait)
	{
		JobsSystemStateSComponent* instance = m_State;
		instance->bBeginSubmition = false;
		Complete(wait);
	}
}