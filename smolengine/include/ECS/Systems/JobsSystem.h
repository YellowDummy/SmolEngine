#pragma once
#include "Core/Core.h"

#include "ECS/Components/Singletons/JobsSystemStateSComponent.h"

namespace SmolEngine
{
	struct JobsSystemStateSComponent;

	enum class JobPriority : uint32_t
	{
		General,
		Rendering
	};

	class JobsSystem
	{
	public:

		static void BeginSubmition();
		static void EndSubmition(bool wait = true);

		template<typename F>
		static void Schedule(F&& f)
		{
			JobsSystemStateSComponent* instance = JobsSystemStateSComponent::GetSingleton();
			instance->Taskflow->emplace(std::forward<F>(f));
		}

	private:

		static void Complete(bool wait);
		static void Clear();

	private:

		inline static JobsSystemStateSComponent* m_State;
		friend class WorldAdmin;
	};
}