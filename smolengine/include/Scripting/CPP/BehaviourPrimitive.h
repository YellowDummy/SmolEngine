#pragma once

#include "ECS/WorldAdmin.h"
#include "ECS/Scene.h"
#include "ECS/Actor.h"

#include "Scripting/FieldManager.h"

#include <string>
#include <vector>
#include <type_traits>

namespace SmolEngine
{
	class Actor;
	class BehaviourPrimitive
	{
	public:

		BehaviourPrimitive() = default;
		virtual ~BehaviourPrimitive() = default;

		// Getters
		const std::string&       GetName();
		const std::string&       GetTag();
		const size_t             GetID();
		void                     GetActors(std::vector<Ref<Actor>>& outList);
		void                     GetActorsWithTag(const std::string& tag, std::vector<Ref<Actor>>& outList);
		uint32_t                 GetChildsCount() const;
		Ref<Actor>               GetChildByName(const std::string& name);
		Ref<Actor>               GetChildByIndex(uint32_t index);
		std::vector<Ref<Actor>>& GetChilds();
		Ref<Actor>               GetParent() const;

		// Search
		Ref<Actor>              FindActorByName(const std::string& name);
		Ref<Actor>              FindActorByTag(const std::string& tag);
		Ref<Actor>              FindActorByID(uint32_t id);


		template<typename T>
		bool CreateValue(const T& intiial_value, const std::string& name)
		{
			T value = intiial_value;
			return m_FieldManager.PushVariable<T>(&value, name);
		}

		template<typename T>
		T* GetValue(const std::string& name)
		{
			Ref<FieldView> view = m_FieldManager.GetVarriable<T>(name);
			if (view)
			{
				return static_cast<T*>(view->ptr);
			}

			return nullptr;
		}

		template<typename T>
		T* GetComponent() { return m_Actor->GetComponent<T>(); }

		template<typename T, typename... Args>
		T* AddComponent(Args&&... args) { return m_Actor->AddComponent<T>(args...); }

		template<typename T>
		bool DestroyComponent() { return m_Actor->DestroyComponent<T>(); }

		template<typename T>
		bool HasComponent() { return m_Actor->HasComponent<T>(); }

	private:
		Ref<Actor>               m_Actor = nullptr;
		FieldManager             m_FieldManager = {};

	private:

		friend class SystemRegistry;
		friend class CollisionListener2D;
		friend class ScriptingSystem;
		friend class WorldAdmin;
		friend class MetaContext;
		friend class Scene;
	};
}