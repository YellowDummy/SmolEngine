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

		// Getetrs
		static JobsSystemStateSComponent* GetSingleton();

		// Data
		bool                                       bBeginSubmition = false;
		inline static JobsSystemStateSComponent*   Instance = nullptr;			      	                                               
		tf::Taskflow*                              Taskflow = nullptr;
		tf::Executor*                              Executor = nullptr;
	};
}