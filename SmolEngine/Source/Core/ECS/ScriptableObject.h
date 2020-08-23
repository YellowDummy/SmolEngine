#pragma once

#include "Core/Core.h"
#include "Core/Time.h"

namespace SmolEngine
{
	class Actor;

	struct ScriptableObject
	{
		bool Enabled = true;

		ScriptableObject(Ref<Actor> actor)
			:m_Actor(actor)
		{
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args) { return m_Actor->AddComponent(args); }

		template<typename T>
		T& GetComponent() { return m_Actor->GetComponent<T>(); }

		template<typename T>
		bool DeleteComponent() { return m_Actor->DeleteComponent<T>(); }

		template<typename T>
		bool HasComponent() { return m_Actor->HasComponent<T>(); }

		Ref<Actor> GetActor() { return m_Actor; }

		virtual void Start() {}
		virtual void OnUpdate(DeltaTime deltaTime) {}
		virtual void OnDestroy() {}
		virtual void OnEndFrame() {}

	private:
		Ref<Actor> m_Actor;
	};
}