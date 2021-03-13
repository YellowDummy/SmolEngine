#include "stdafx.h"
#include "JobsSystem.h"

#include <thread>
#include <algorithm>

namespace SmolEngine
{
	void JobsSystem::Complete(bool wait)
	{
		JobsSystemStateSComponent* instance = JobsSystemStateSComponent::GetSingleton();
		sort(instance->JobGroupIDs.begin(), instance->JobGroupIDs.end());

		for (uint32_t i = 0; i < static_cast<uint32_t>(instance->JobGroupIDs.size()); ++i)
		{
			uint32_t id = instance->JobGroupIDs[i];
			for (auto& job : instance->SheduleTasks[id])
			{
				instance->ThreadPoolInstance->SubmitWork((WorkerSpecialization)job->State, job->Func);
			}

			if (wait)
			{
				for (auto& job : instance->SheduleTasks[id])
					job->Ptr.wait();
			}
		}
	}

	void JobsSystem::Clear()
	{
		JobsSystemStateSComponent* instance = JobsSystemStateSComponent::GetSingleton();
		for (auto& [key, dsc] : instance->SheduleTasks)
		{
			for (auto& job : dsc)
				delete job; 
		}

		instance->JobGroupIDs.clear();
		instance->SheduleTasks.clear();
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

	ThreadPool* JobsSystem::GetThreadPool()
	{
		return JobsSystemStateSComponent::GetSingleton()->ThreadPoolInstance;
	}
}