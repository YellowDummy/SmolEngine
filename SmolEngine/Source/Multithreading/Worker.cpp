#include "stdafx.h"
#include "Worker.h"

#include "Core/SLog.h"
#include "Multithreading/ThreadPool.h"

namespace SmolEngine
{
    void Worker::Init(WorkerCreateInfo* info)
    {
        auto fn = [](ThreadPool* pool, WorkerState* state)
        {
            for (;;)
            {
                std::function<void()> task;
                {
                    auto& targetPool = pool->m_Pools[(uint32_t)state->Specialization];
                    std::unique_lock<std::mutex> lock(*targetPool.Mutex);
                    auto& queue = targetPool.Tasks;

                    auto predicate = [&]
                    {
#if  0
                        switch (info->State->Specialization)
                        {
                        case WorkerSpecialization::None:
                        {
                            break;
                        }
                        case WorkerSpecialization::Rendering:
                        {
                            break;
                        }
                        default:
                            break;
                        }
                        NATIVE_ERROR("Waiting!");
#endif 
                        return pool->m_bStop || !queue.empty();
                    };

                    targetPool.Condition->wait(lock, predicate);
                    if (pool->m_bStop || queue.empty())
                    {
                        NATIVE_ERROR("Exit!");
                        return;
                    }

                    task = std::move(queue.front());
                    queue.pop();
                }

                state->bWorking = true;
                NATIVE_INFO("Worker with ID {} starts execution", state->ID);
                task();
                state->bWorking = false;
            }
        };

        m_Thread = std::thread(fn, info->Pool, info->State);
    }

    void Worker::Join()
    {
        m_Thread.join();
    }
}