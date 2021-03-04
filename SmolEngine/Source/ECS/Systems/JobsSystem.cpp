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
				instance->ThreadPool.SubmitWork((WorkerSpecialization)job->State, job->Func);
			}

			if (wait)
			{
				auto last = instance->SheduleTasks[id].size() - 1;
				instance->SheduleTasks[id][last]->Ptr.wait();
			}

			for (auto& job : instance->SheduleTasks[id])
				delete job;
		}

		instance->JobGroupIDs.clear();
		instance->SheduleTasks.clear();
	}
}