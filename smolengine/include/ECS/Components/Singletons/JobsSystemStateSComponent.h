#pragma once
#include "Core/Core.h"

#include <taskflow/taskflow/include/taskflow.hpp>
#include <unordered_map>

namespace SmolEngine
{
	struct JobsSystemStateSComponent
	{
		JobsSystemStateSComponent();
		~JobsSystemStateSComponent();
		// Dummy c-tors - required by EnTT
		JobsSystemStateSComponent(JobsSystemStateSComponent& another) {}
		JobsSystemStateSComponent(JobsSystemStateSComponent&& other) {}
		JobsSystemStateSComponent& operator=(JobsSystemStateSComponent other) { return *this; }

		// Getetrs
		static JobsSystemStateSComponent* GetSingleton();

		// Data
		bool                                       bBeginSubmition = false;	      	                                               
		tf::Taskflow                               Taskflow;
		tf::Executor                               Executor;

	private:
		inline static JobsSystemStateSComponent* Instance = nullptr;
	};
}