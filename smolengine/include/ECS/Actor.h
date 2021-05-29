#pragma once

#include "ECS/Scene.h"
#include "ECS/WorldAdmin.h"

#include <functional>
#include <vector>
#include <unordered_map>

#include <entt/entity/registry.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>

namespace SmolEngine
{
	struct OutValue;
	struct DefaultBaseTuple;
	struct PhysicsBaseTuple;
	struct CameraBaseTuple;
	struct ResourceTuple;
	struct HeadComponent;

	class Actor
	{
	public:

		Actor() = default;
		operator entt::entity() const { return m_Entity; }

		// Getters
		const std::string&           GetName() const;
		const std::string&           GetTag() const;
		uint32_t                     GetComponentsCount() const;
		const HeadComponent*         GetHead() const;
		uint32_t                     GetID() const;
		uint32_t                     GetChildsCount() const;
		Actor*                       GetChildByName(const std::string& name);
		Actor*                       GetChildByIndex(uint32_t index);
		std::vector<Actor*>&         GetChilds();
		Actor*                       GetParent() const;
		Actor*                       GetRootActor();
		Actor*                       FindActorByID(uint32_t id);

		// Setters
		bool                         SetParent(Actor* parent);
		bool                         SetChild(Actor* child);
		bool                         SetName(const std::string& name);
		bool                         RemoveChildAtIndex(uint32_t index);

		template<typename T>
		T* GetComponent()            { return WorldAdmin::GetSingleton()->GetActiveScene()->GetComponent<T>(m_Entity); }
		template<typename T>
		bool HasComponent()          { return WorldAdmin::GetSingleton()->GetActiveScene()->HasComponent<T>(m_Entity); }
		template<typename T, typename... Args>
		T* AddComponent(Args&&... args) { return WorldAdmin::GetSingleton()->GetActiveScene()->AddComponent<T>(this, args...); }

	private:

		HeadComponent*               GetInfo();
		Actor*                       FindRootActor(Actor* actor);
		entt::entity                 m_Entity{};

	private:
		friend class cereal::access;
		friend struct ScriptableObject;
		friend class WorldAdmin;
		friend class Scene;
		friend class EditorLayer;
		friend class ScriptingSystem;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(m_Entity);
		}

	};
}