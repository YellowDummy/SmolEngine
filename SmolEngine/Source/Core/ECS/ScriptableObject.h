#pragma once

#include "Core/Core.h"
#include "Core/Time.h"

namespace SmolEngine
{
	class Actor;

	struct ScriptableObject 	//Base Class *Sturct* For All Script Classes
	{
		bool Enabled = true;

		ScriptableObject(Ref<Actor> actor)
			:m_Actor(actor)
		{
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args) { return m_Actor->AddComponent<T>(args); }

		template<typename T>
		T& GetComponent() { return m_Actor->GetComponent<T>(); }

		template<typename T>
		bool DeleteComponent() { return m_Actor->DeleteComponent<T>(); }

		template<typename T>
		bool HasComponent() { return m_Actor->HasComponent<T>(); }

		std::string& GetName();
		const std::string& GetTag();

		Ref<Actor> GetParent();
		void SetParent(Ref<Actor> parent);

		std::vector<Ref<Actor>>& GetChilds();
		std::vector<Ref<Actor>>& GetActorList();
		std::vector<Ref<Actor>>  GetActorListByTag(const std::string& tag);

		void AddChild(Ref<Actor> child);
		void RemoveChild(Ref<Actor> child);

		Ref<Actor> GetChildByName(const std::string& name);
		Ref<Actor> GetChildByTag(const std::string& tag);
		Ref<Actor> GetActorByName(const std::string& name);
		Ref<Actor> GetActorByTag(const std::string& tag);
		Ref<Actor> GetActor() { return m_Actor; }

		virtual void Start() {}
		virtual void OnUpdate(DeltaTime deltaTime) {}
		virtual void OnDestroy() {}
		virtual void OnEndFrame() {}

	private:
		Ref<Actor> m_Actor;
	};
}