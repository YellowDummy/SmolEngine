#pragma once
#include "Core/Core.h"

#include <thread>

namespace SmolEngine
{
	class ThreadPool;
	enum class WorkerSpecialization : uint32_t
	{
		None,
		Rendering
	};

	struct WorkerState
	{
		bool                  bWorking = false;
		uint32_t              ID = 0;
		WorkerSpecialization  Specialization = WorkerSpecialization::None;
	};

	struct WorkerCreateInfo
	{
		ThreadPool*           Pool = nullptr;
		WorkerState*          State = nullptr;
	};

	class Worker
	{
	public:

		void Init(WorkerCreateInfo* info);

		void Join();

	private:

		std::thread           m_Thread;
	};
}