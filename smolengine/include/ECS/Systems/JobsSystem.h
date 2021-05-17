#pragma once
#include "Core/Core.h"

#include "ECS/Components/Singletons/JobsSystemStateSComponent.h"

namespace SmolEngine
{
	struct JobsSystemStateSComponent;

	class JobsSystem
	{
	public:

		static void BeginSubmition(QueueType type = QueueType::PRIMARY);
		static void EndSubmition(QueueType type = QueueType::PRIMARY, bool wait = true);

		template<typename... F>
		static void Schedule(F&&... f)
		{
			switch (m_State->Type)
			{
			case QueueType::PRIMARY: m_State->QueuePrimary.emplace(std::forward<F>(f)...); break;
			case QueueType::SECONDARY: m_State->QueueSecondary.emplace(std::forward<F>(f)...); break;
			}
		}

	private:

		inline static JobsSystemStateSComponent* m_State;
		friend class WorldAdmin;
	};
}