#pragma once
#include "Core/Core.h"

#include "Multithreading/ThreadPool.h"

#include <unordered_map>
#include <future>
#include <any>

namespace SmolEngine
{
	struct JobDescription
	{
		uint32_t              State;
		std::function<void()> Func;
		std::future<void>     Ptr;
	};

	struct JobsSystemStateSComponent
	{
		JobsSystemStateSComponent();

		~JobsSystemStateSComponent();

		// Getetrs
		static JobsSystemStateSComponent* GetSingleton();

		// Data
		inline static JobsSystemStateSComponent*                   Instance = nullptr;			      
		ThreadPool*                                                ThreadPoolInstance = nullptr;
		bool                                                       bBeginSubmition = false;
		std::vector<uint32_t>                                      JobGroupIDs;
		std::unordered_map<uint32_t,std::vector<JobDescription*>>  SheduleTasks;
	};
}