#include "stdafx.h"
#include "ECS/Systems/JobsSystem.h"

namespace SmolEngine
{
	void JobsSystem::BeginSubmition(QueueType type)
	{
		tf::Taskflow* queue = nullptr;
		m_State->Type = type;
		switch (type)
		{
		case QueueType::PRIMARY: queue = &m_State->QueuePrimary; break;
		case QueueType::SECONDARY: queue = &m_State->QueueSecondary; break;
		}

		queue->clear();
	}

	void JobsSystem::EndSubmition(QueueType type, bool wait)
	{
		tf::Executor* executor = m_State->Executor;
		tf::Taskflow* queue = nullptr;

		switch (type)
		{
		case QueueType::PRIMARY: queue = &m_State->QueuePrimary; break;
		case QueueType::SECONDARY: queue = &m_State->QueueSecondary; break;
		}

		executor->run(*queue);
		if (wait)
			executor->wait_for_all();
	}
}