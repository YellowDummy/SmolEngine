#pragma once
#include "Core/Core.h"

#include "ECS/Actor.h"
#include "ECS/SceneRegisrty.h"

namespace SmolEngine
{
	struct SceneStateComponent;

	class Scene
	{
	public:

		Scene() = default;
		~Scene() = default;
		Scene(const Scene& another);

		void                    OnUpdate(DeltaTime delta);
		bool                    Save(const std::string& filePath);
		bool                    Load(const std::string& filePath);
		void                    DuplicateActor(Ref<Actor>& actor);
		void                    DeleteActor(Ref<Actor>& actor);
		Ref<Actor>              CreateActor(const std::string& name, const std::string& tag = std::string("Default"));
		Ref<Actor>              FindActorByName(const std::string& name);
		Ref<Actor>              FindActorByTag(const std::string& tag);
		Ref<Actor>              FindActorByID(uint32_t id);

		void                    GetActors(std::vector<Ref<Actor>>& outList);
		void                    GetActorsByTag(const std::string& tag, std::vector<Ref<Actor>>& outList);
		SceneStateComponent*    GetSceneState();
		entt::registry&         GetRegistry();

		template<typename T, typename... Args>
		T* AddComponentEX(Actor* actor, Args&&... args)
		{
			entt::entity ent = actor->m_Entity;
			HeadComponent* head = actor->GetInfo();

			if (HasComponent<T>(ent))
			{
				NATIVE_ERROR("Actor already have component!");
				return nullptr;
			}

			auto& component = m_SceneData.m_Registry.emplace<T>(ent, std::forward<Args>(args)...);
			component.ComponentID = head->ComponentsCount;
			head->ComponentsCount++;
			return &component;
		}

		template<typename T, typename... Args>
		T* AddComponent(Ref<Actor>& actor, Args&&... args)
		{
			if (HasComponent<T>(actor))
			{
				NATIVE_ERROR("Actor already have component!");
				return nullptr;
			}

			HeadComponent* head = actor->GetInfo();
			entt::entity ent = actor->m_Entity;
			auto& component = m_SceneData.m_Registry.emplace<T>(ent, std::forward<Args>(args)...);
			component.ComponentID = head->ComponentsCount;
			head->ComponentsCount++;
			return &component;
		}

		template<typename T>
		bool HasComponent(Ref<Actor>& actor)
		{
			entt::entity ent = actor->m_Entity;
			return m_SceneData.m_Registry.try_get<T>(ent) != nullptr;
		}

		template<typename T>
		bool HasComponent(entt::entity ent)
		{
			return m_SceneData.m_Registry.try_get<T>(ent) != nullptr;
		}

		template<typename T>
		T* GetComponent(entt::entity ent)
		{
			return m_SceneData.m_Registry.try_get<T>(ent);
		}

		template<typename T>
		T* GetComponent(Ref<Actor>& actor)
		{
			entt::entity ent = actor->m_Entity;
			return m_SceneData.m_Registry.try_get<T>(ent);
		}

		template<typename T>
		T* GetComponentEX(Actor* actor)
		{
			entt::entity ent = actor->m_Entity;
			return m_SceneData.m_Registry.try_get<T>(ent);
		}

		bool AddScript(Ref<Actor>& actor, const std::string& script_name);

	private:

		void                    Create(const std::string& filePath);
		void                    Free();
		void                    CalculateRelativePositions();
		void                    UpdateChildsPositions();

		SceneStateComponent*    GetStateComponent();
		void                    CleanRegistry();

	private:
		SceneRegisrty           m_SceneData;
		SceneStateComponent*    m_State = nullptr;
	private:				    
		friend class WorldAdmin;
		friend class EditorLayer;
	};
}