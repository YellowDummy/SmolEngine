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
		T* AddComponent(Ref<Actor>& actor, Args&&... args)
		{
			return AddComponent<T>(actor->m_Entity, args...);
		}

		template<typename T, typename... Args>
		T* AddComponent(entt::entity entt_id, Args&&... args)
		{
			if (HasComponent<T>(entt_id))
			{
				NATIVE_ERROR("Actor already have component!");
				return nullptr;
			}

			HeadComponent* head = GetComponent<HeadComponent>(entt_id);
			auto& component = m_SceneData.m_Registry.emplace<T>(entt_id, std::forward<Args>(args)...);
			component.ComponentID = head->ComponentsCount;
			head->ComponentsCount++;
			return &component;
		}

		template<typename T>
		bool DestroyComponent(Ref<Actor>& actor)
		{
			return DestroyComponent<T>(actor->m_Entity);
		}

		template<typename T>
		bool DestroyComponent(entt::entity entt_id)
		{
			if (HasComponent<T>(entt_id))
			{
				m_SceneData.m_Registry.remove<T>(entt_id);
				return true;
			}

			return false;
		}

		template<typename T>
		bool HasComponent(Ref<Actor>& actor)
		{
			return HasComponent<T>(actor->m_Entity);
		}

		template<typename T>
		bool HasComponent(entt::entity entt_id)
		{
			return m_SceneData.m_Registry.try_get<T>(entt_id) != nullptr;
		}

		template<typename T>
		T* GetComponent(Ref<Actor>& actor)
		{
			return GetComponent<T>(actor->m_Entity);
		}

		template<typename T>
		T* GetComponent(entt::entity entt_id)
		{
			return m_SceneData.m_Registry.try_get<T>(entt_id);
		}

		bool AddCppScript(Ref<Actor>& actor, const std::string& script_name);
		bool AddCSharpScript(Ref<Actor>& actor, const std::string& class_name);

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