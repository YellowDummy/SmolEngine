#pragma once
#include "Core/Core.h"

#include <taskflow/include/taskflow.hpp>
#include <unordered_map>

namespace SmolEngine
{
	enum class QueueType
	{
		PRIMARY,
		SECONDARY
	};

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
		QueueType                                  Type = QueueType::PRIMARY;
		tf::Executor*                              Executor = nullptr;
		tf::Taskflow                               QueuePrimary;
		tf::Taskflow                               QueueSecondary;

	private:
		inline static JobsSystemStateSComponent*   Instance = nullptr;
	};
}