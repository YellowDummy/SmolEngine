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
                {;
                    std::unique_lock<std::mutex> lock(pool->m_Mutex);
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
                        return pool->m_bStop || !pool->m_Tasks.empty();
                    };

                    pool->m_Condition.wait(lock, predicate);
                    if (pool->m_bStop || pool->m_Tasks.empty())
                    {
                        NATIVE_ERROR("Exit!");
                        return;
                    }

                    task = std::move(pool->m_Tasks.front());
                    pool->m_Tasks.pop();
                }

                state->bWorking = true;
                //NATIVE_INFO("Worker with ID {} starts execution", state->ID);
                if (task)
                {
                    task();
                }
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