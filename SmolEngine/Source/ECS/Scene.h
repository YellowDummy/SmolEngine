#pragma once

#include "Core/Core.h"
#include "Core/SLog.h"

#include "ECS/SceneData.h"

namespace SmolEngine
{
	enum class ActorBaseType : uint16_t;
	class Actor;
	struct BehaviourComponent;

	class Scene
	{
	public:

		Scene() = default;

		void Init(const std::string& filePath);

		/// Actors

		Ref<Actor> CreateActor(const ActorBaseType baseType, const std::string& name, const std::string& tag = std::string("Default"));

		// Search

		Ref<Actor> FindActorByName(const std::string& name);

		Ref<Actor> FindActorByTag(const std::string& tag);

		Ref<Actor> FindActorByID(const uint32_t id);

		// Operations

		void RemoveChild(Ref<Actor>& parent, Ref<Actor>& child);

		void AddChild(Ref<Actor>& parent, Ref<Actor>& child);

		void DuplicateActor(Ref<Actor>& actor);

		void DeleteActor(Ref<Actor>& actor);

		// Save / Load

		bool Save(const std::string& filePath);

		bool Load(const std::string& filePath);

		// Components

		BehaviourComponent* AddBehaviour(const std::string& systemName, const Ref<Actor>& actor);

		template<typename T, typename... Args>
		T* AddComponent(Actor& actor, Args&&... args)
		{
			if (HasComponent<T>(actor))
			{
				NATIVE_ERROR("Actor already have component!");
				return nullptr;
			}

			auto& component = m_SceneData.m_Registry.emplace<T>(actor, std::forward<Args>(args)...);
			component.ComponentID = actor.m_ComponentsCount;
			actor.m_ComponentsCount++;
			return &component;
		}

		template<typename T>
		T* AddTuple(Actor& actor)
		{
			if (HasComponent<T>(actor))
			{
				NATIVE_ERROR("Actor already have tuple!");
				return nullptr;
			}

			auto& component = m_SceneData.m_Registry.emplace<T>(actor);
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

		// Helpers

		bool OnActorNameChanged(const std::string& lastName, const std::string& newName);

		void DeleteSingletons();

		void LoadSingletons();

		void UpdateIDSet();

		void CleanRegistry();

		// Assets

		bool AddAsset(const std::string& fileName, const std::string& filePath);

		bool DeleteAsset(const std::string& fileName);

		// Getters

		entt::registry& GetRegistry() { return m_SceneData.m_Registry; }

		const std::unordered_map<std::string, std::string>& GetAssetMap();

		std::unordered_map<size_t, Ref<Actor>>& GetActorPool();


		std::vector<Ref<Actor>> GetActorList();

		std::vector<Ref<Actor>> GetSortedActorList();

		std::vector<Ref<Actor>> GetActorListByTag(const std::string& tag);


		SceneData& GetSceneData();

		std::unordered_map<std::string, size_t>& GetIDSet();

	private:

		SceneData m_SceneData;
		std::unordered_map<std::string, size_t> m_IDSet;

	private:

		friend class WorldAdmin;
	};
}