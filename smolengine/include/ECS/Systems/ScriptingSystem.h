#pragma once
#include "Core/Core.h"
#include "ECS/Components/Singletons/ScriptingSystemStateSComponent.h"
#include "Scripting/CPP/MetaContext.h"

#ifndef FROSTIUM_SMOLENGINE_IMPL
#define FROSTIUM_SMOLENGINE_IMPL
#endif
#include <Frostium3D/Common/Time.h>

namespace SmolEngine
{
	struct WorldAdminStateSComponent;
	struct ScriptingSystemStateSComponent;
	struct CSharpScriptComponent;
	class Actor;

	class ScriptingSystem
	{
	public:
		static bool AttachNativeScript(Ref<Actor>& actor, const std::string& scriptName);
		static bool AttachCSharpScript(Ref<Actor>& actor, const std::string& className);

		template<typename T>
		bool AddNativeClass(const std::string& name)
		{
			return m_State->m_MetaContext->AddClass<T>(name);
		}

	private:
		static void OnBeginWorld();
		static void OnEndWorld();
		static void OnDestroy(Actor* actor);

		static void OnUpdate(DeltaTime deltaTime);
		static void OnCollisionBegin(Actor* actorB, Actor* actorA, bool isTrigger);
		static void OnCollisionEnd(Actor* actorB, Actor* actorA, bool isTrigger);
		static void OnSceneReloaded(void* registry_);

		static void CreateScripts();
		static void CreateScript(CSharpScriptComponent* comp);
		static void ClearScripts();

		template<typename T>
		static T* GetOrCreateComponent(Ref<Actor>& actor)
		{
			if (WorldAdmin::GetSingleton()->GetActiveScene()->HasComponent<T>(actor) == false)
			{
				return WorldAdmin::GetSingleton()->GetActiveScene()->AddComponent<T>(actor);
			}
			else
			{
				return WorldAdmin::GetSingleton()->GetActiveScene()->GetComponent<T>(actor);
			}
		}

	private:
		inline static ScriptingSystemStateSComponent*  m_State = nullptr;
		inline static WorldAdminStateSComponent*       m_World = nullptr;

		friend class WorldAdmin;
		friend class Scene;
		friend class CollisionListener2D;
		friend class ComponentHandler;
	};
}