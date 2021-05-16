#pragma once
#ifdef FROSTIUM_SMOLENGINE_IMPL

#include "Common/Core.h"

#include <taskflow/taskflow/include/taskflow.hpp>

namespace Frostium
{
	class JobsSystem
	{
	public:
		JobsSystem();

		static void             BeginSubmition();
		static void             EndSubmition(bool wait = true);
						        
		static uint32_t         GetNumWorkers();
		static uint32_t         GetNumRenderingTasks();
		static tf::Executor*    GetExecutor();
							    
		template<typename... F>    
		static void             Schedule(F&&... f) { s_Instance->m_RenderingQueue.emplace(std::forward<F>(f)...); }

	private:

		static JobsSystem*      s_Instance;
		tf::Taskflow            m_RenderingQueue{};
		tf::Executor            m_Executor{};
	};						                 
}
#endif