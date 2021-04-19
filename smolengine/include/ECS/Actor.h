#pragma once


#include <functional>
#include <vector>
#include <unordered_map>

#include <entt/entt.hpp>
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

		Actor();
		Actor(entt::entity entity, size_t index);
		operator entt::entity() const { return m_Entity; }

		// Getters
		const std::string& GetName() const;
		const std::string& GetTag() const;
		const uint32_t GetID() const;
		const size_t GetComponentsCount() const;

		std::vector<Actor*>& GetChilds();
		Actor* GetChildByName(const std::string& name);
		Actor* GetChildByTag(const std::string& tag);
		Actor* GetParent();

		// Setters
		void SetParent(Actor* parent);
		const HeadComponent* GetInfo() const;

		template<typename T>
		T* GetComponent()
		{
			return WorldAdmin::GetSingleton()->GetActiveScene()->GetComponent<T>(m_Entity);
		}

		template<typename T>
		bool HasComponent()
		{
			return WorldAdmin::GetSingleton()->GetActiveScene()->HasComponent<T>(m_Entity);
		}

		template<typename T, typename... Args>
		T* AddComponent(Args&&... args)
		{
			return WorldAdmin::GetSingleton()->GetActiveScene()->AddComponent<T>(this, args...);
		}

	private:

		bool                             m_showComponentUI = false;
		Actor*                           m_Parent = nullptr;
		entt::entity                     m_Entity;
		size_t                           m_Index = 0;
		size_t                           m_ComponentsCount = 0;
		std::vector<Actor*>              m_Childs;

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
			archive(m_Entity, m_Index, m_ComponentsCount);
		}

	};
}