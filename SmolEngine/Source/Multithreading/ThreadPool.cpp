#include "stdafx.h"
#include "ThreadPool.h"

#include "Multithreading/Worker.h"

namespace SmolEngine
{
    ThreadPool::ThreadPool()
    {
        uint32_t numThreads = std::thread::hardware_concurrency();

        m_Pools.resize(2);
        m_Workers.resize(numThreads);

        WorkerCreateInfo workerCI = {};
        for (uint32_t i = 0; i < numThreads; ++i)
        {
            m_Workers[i].State.bWorking = false;
            m_Workers[i].State.ID = i;
            m_Workers[i].State.Specialization = i == 0 ?
                WorkerSpecialization::Rendering : WorkerSpecialization::None;

            workerCI.Pool = this;
            workerCI.State = &m_Workers[i].State;

            m_Workers[i].Unit.Init(&workerCI);
        }

        NATIVE_INFO("ThreadPool -> avalable thareads: {}", numThreads);
    }

    ThreadPool::~ThreadPool()
    {
        m_bStop = true;

        for (auto& pool : m_Pools)
        {
            std::unique_lock<std::mutex> lock(*pool.Mutex);
            pool.Condition->notify_all();
        }

        for (auto& worker : m_Workers)
            worker.Unit.Join();
    }

    uint32_t ThreadPool::GetNumWorkers() const
    {
        return static_cast<uint32_t>(m_Workers.size());
    }

    uint32_t ThreadPool::GetNumActiveWorkers() const
    {
        uint32_t num = 0;
        for (auto& worker : m_Workers)
        {
            if (worker.State.bWorking)
                num++;
        }

        return num;
    }

    uint32_t ThreadPool::GetNumWaitingWorkers() const
    {
        uint32_t num = 0;
        for (auto& worker : m_Workers)
        {
            if (!worker.State.bWorking)
                num++;
        }

        return num;
    }
}