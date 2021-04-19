#pragma once
#include "Core/Core.h"

#include "ECS/SceneData.h"
#include "ECS/Actor.h"

namespace SmolEngine
{
	class Actor;
	struct BehaviourComponent;

	class Scene
	{
	public:

		Scene();
		~Scene();
		Scene(const Scene& another);

		void Init(const std::string& filePath);
		void Free();

		Ref<Actor> CreateActor(const std::string& name, const std::string& tag = std::string("Default"));
		Ref<Actor> FindActorByName(const std::string& name);
		Ref<Actor> FindActorByTag(const std::string& tag);
		Ref<Actor> FindActorByID(const uint32_t id);

		void RemoveChild(Actor* parent, Actor* child);
		void AddChild(Actor* parent, Actor* child);
		void DuplicateActor(Ref<Actor>& actor);
		void DeleteActor(Ref<Actor>& actor);

		bool Save(const std::string& filePath);
		bool Load(const std::string& filePath);

		template<typename T, typename... Args>
		T* AddComponent(Actor& actor, Args&&... args)
		{
			if (HasComponent<T>(actor))
			{
				NATIVE_ERROR("Actor already have component!");
				return nullptr;
			}

			auto& component = m_SceneData.m_Registry.emplace<T>(actor, std::forward<Args>(args)...);
			component.ComponentID = static_cast<uint32_t>(actor.m_ComponentsCount);
			actor.m_ComponentsCount++;
			return &component;
		}

		template<typename T, typename... Args>
		T* AddComponent(Actor* actor, Args&&... args)
		{
			if (HasComponent<T>(*actor))
			{
				NATIVE_ERROR("Actor already have component!");
				return nullptr;
			}

			auto& component = m_SceneData.m_Registry.emplace<T>(*actor, std::forward<Args>(args)...);
			component.ComponentID = static_cast<uint32_t>(actor->m_ComponentsCount);
			actor->m_ComponentsCount++;
			return &component;
		}

		template<typename T>
		bool HasComponent(entt::entity entity)
		{
			return m_SceneData.m_Registry.has<T>(entity);
		}

		template<typename T>
		T* GetComponent(entt::entity entity)
		{
			if (!HasComponent<T>(entity))
			{
				NATIVE_ERROR("Actor does not have component");
				return nullptr;
			}

			auto& component = m_SceneData.m_Registry.get<T>(entity);
			return &component;
		}

		bool OnActorNameChanged(const std::string& lastName, const std::string& newName);
		bool AddAsset(const std::string& fileName, const std::string& filePath);
		bool DeleteAsset(const std::string& fileName);
		void UpdateIDSet();
		void CleanRegistry();

		// Getters
		entt::registry& GetRegistry() { return m_SceneData.m_Registry; }
		const std::unordered_map<std::string, std::string>& GetAssetMap();
		std::unordered_map<uint32_t, Ref<Actor>>& GetActorPool();
		void GetActorList(std::vector<Ref<Actor>>& outList);
		void GetSortedActorList(std::vector<Ref<Actor>>& outList);
		void GetActorListByTag(const std::string& tag, std::vector<Ref<Actor>>& outList);
		SceneData& GetSceneData();
		std::unordered_map<std::string, uint32_t>& GetIDSet();

	private:

		SceneData                                  m_SceneData;
		std::unordered_map<std::string, uint32_t>  m_IDSet;

	private:

		friend class WorldAdmin;
	};
}