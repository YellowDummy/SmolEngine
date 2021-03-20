#pragma once
#include "Core/Core.h"

#include "Multithreading/ThreadPool.h"

#include <taskflow/include/taskflow.hpp>
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
		bool                                                       bBeginSubmition = false;

		tf::Taskflow* Taskflow = nullptr;
		tf::Executor* Executor = nullptr;
	};
}