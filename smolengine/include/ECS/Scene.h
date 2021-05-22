#pragma once
#include "Core/Core.h"

#include "ECS/Actor.h"
#include "ECS/SceneRegisrty.h"

namespace SmolEngine
{
	struct BehaviourComponent;
	struct SceneStateComponent;

	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;
		Scene(const Scene& another);

		bool Save(const std::string& filePath);
		bool Load(const std::string& filePath);

		void RemoveChild(Actor* parent, Actor* child);
		void AddChild(Actor* parent, Actor* child);
		void DuplicateActor(Actor* actor);
		void DeleteActor(Actor* actor);

		Actor* CreateActor(const std::string& name, const std::string& tag = std::string("Default"));
		Actor* FindActorByName(const std::string& name);
		Actor* FindActorByTag(const std::string& tag);
		Actor* FindActorByID(uint32_t id);

		// Getters
		void GetActors(std::vector<Actor*>& outList);
		void GetActorsByID(std::vector<Actor*>& outList);
		void GetActorsByTag(const std::string& tag, std::vector<Actor*>& outList);
		SceneStateComponent* GetSceneState();
		entt::registry& GetRegistry();

		template<typename T, typename... Args>
		T* AddComponent(Actor* actor, Args&&... args)
		{
			if (HasComponent<T>(actor))
			{
				NATIVE_ERROR("Actor already have component!");
				return nullptr;
			}

			HeadComponent* head = actor->GetInfo();
			auto& component = m_SceneData.m_Registry.emplace<T>(*actor, std::forward<Args>(args)...);
			component.ComponentID = head->ComponentsCount;
			head->ComponentsCount++;
			return &component;
		}

		template<typename T>
		bool HasComponent(Actor* actor)
		{
			return m_SceneData.m_Registry.try_get<T>(*actor) != nullptr;
		}

		template<typename T>
		bool HasComponent(entt::entity ent)
		{
			return m_SceneData.m_Registry.try_get<T>(ent) != nullptr;
		}

		template<typename T>
		T* GetComponent(entt::entity ent)
		{
			if (!HasComponent<T>(ent))
			{
				NATIVE_ERROR("Actor does not have component");
				return nullptr;
			}

			return &m_SceneData.m_Registry.get<T>(ent);
		}

		template<typename T>
		T* GetComponent(Actor* actor)
		{
			if (!HasComponent<T>(actor))
			{
				NATIVE_ERROR("Actor does not have component");
				return nullptr;
			}

			return &m_SceneData.m_Registry.get<T>(*actor);
		}

		bool AddScript(Actor* actor, const std::string& script_name);

	private:
		void Create(const std::string& filePath);
		void Free();

		SceneStateComponent* GetStateComponent();
		void CleanRegistry();

	private:
		SceneRegisrty          m_SceneData;
		SceneStateComponent*   m_State = nullptr;
	private:
		friend class WorldAdmin;
		friend class EditorLayer;
	};
}