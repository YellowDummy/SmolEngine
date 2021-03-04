#pragma once
#include "Core/Core.h"
#include "Multithreading/Worker.h"

#include <vector>
#include <queue>
#include <mutex>
#include <future>
#include <functional>
#include <condition_variable>
#include <stdexcept>

namespace SmolEngine
{
	class ThreadPool
	{
	public:

		ThreadPool();

		~ThreadPool();

		// Getters

		bool IsAnyWorkerBusy() const;

		uint32_t GetNumWorkers() const;

		uint32_t GetNumActiveWorkers() const;

		uint32_t GetNumWaitingWorkers() const;

		// Submit

		template<typename F, typename...Args>
		auto SubmitWork(WorkerSpecialization workSpec, F&& f,  Args&&... args) -> std::future<decltype(f(args...))>
		{
			// Create a function with bounded parameters ready to execute
			std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

			// Encapsulate it into a shared ptr in order to be able to copy construct / assign 
			auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

			// Wrap packaged task into void function
			std::function<void()> wrapper_func = [task_ptr]() {
				(*task_ptr)();
			};

			m_Pools[(uint32_t)workSpec].Tasks.emplace(wrapper_func);
			m_Pools[(uint32_t)workSpec].Condition->notify_one();

			return task_ptr->get_future();
		}

		void SubmitWork(WorkerSpecialization workSpec, std::function<void()>& func);

	private:

		struct PoolData
		{
			PoolData()
				: Mutex(new std::mutex()), Condition(new std::condition_variable()) {}

			~PoolData()
			{
				delete Mutex;
				delete Condition;
			}

			PoolData(const PoolData& data)
			{
				Mutex = Mutex;
				Condition = Condition;
				Tasks = Tasks;
			}

			// Data

			std::mutex*                                 Mutex = nullptr;
			std::condition_variable*                    Condition = nullptr;
			std::queue<std::function<void()>>           Tasks;
		};

		struct WorkerUnit
		{
			Worker                                      Unit;
			WorkerState                                 State;
		};

		bool                                            m_bStop = false;
		std::vector<PoolData>                           m_Pools;
		std::vector<WorkerUnit*>                        m_Workers;

	private:

		friend class Worker;
	};
}