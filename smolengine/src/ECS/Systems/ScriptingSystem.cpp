#include "stdafx.h"
#include "ECS/Systems/ScriptingSystem.h"
#include "ECS/Components/ScriptComponent.h"
#include "ECS/Components/HeadComponent.h"
#include "ECS/Components/Singletons/WorldAdminStateSComponent.h"
#include "ECS/WorldAdmin.h"

#include "Scripting/CPP/BehaviourPrimitive.h"
#include "Scripting/CPP/MetaContext.h"
#include "Scripting/CSharp/MonoContext.h"

namespace SmolEngine
{
	// 19.07.2021
	// TODO: create interface 

	bool ScriptingSystem::AttachNativeScript(Ref<Actor>& actor, const std::string& scriptName)
	{
		MetaContext* meta_context = m_State->m_MetaContext;
		auto& it = meta_context->m_MetaMap.find(scriptName);
		if (it == meta_context->m_MetaMap.end())
			return false;

		ScriptComponent* component = GetOrCreateComponent<ScriptComponent>(actor);
		component->pActor = actor;

		int32_t index = static_cast<int32_t>(actor->GetComponentsCount());
		actor->GetInfo()->ComponentsCount++;

		auto& primitive = it->second.ClassInstance.cast<BehaviourPrimitive>();

		ScriptComponent::CPPInstance scriptInstance = {};
		scriptInstance.Name = scriptName;
		scriptInstance.Instance= it->second.ClassInstance;
		scriptInstance.Fields = primitive.m_FieldManager;

		primitive.m_Actor = actor;
		component->CppScripts.emplace_back(scriptInstance);
		return true;
	}

	bool ScriptingSystem::AttachCSharpScript(Ref<Actor>& actor, const std::string& className)
	{
		MonoContext* mono = m_State->m_MonoContext;
		auto& it = mono->m_MetaMap.find(className);
		if (it == mono->m_MetaMap.end())
			return false;

		const auto& meta_ = it->second;
		auto* component = GetOrCreateComponent<ScriptComponent>(actor);

		ScriptComponent::CSharpInstance scriptInstance = {};
		scriptInstance.Fields = meta_.Fields;;
		scriptInstance.Name = className;

		component->pActor = actor;
		component->CSharpScripts.emplace_back(scriptInstance);
		return true;
	}

	void ScriptingSystem::OnBeginWorld()
	{
		entt::registry* reg = m_World->m_CurrentRegistry;
		const auto& view = reg->view<ScriptComponent>();
		for (const auto& entity : view)
		{
			auto& component = view.get<ScriptComponent>(entity);

			m_State->m_MetaContext->OnBegin(&component);
			m_State->m_MonoContext->OnBegin(&component);
		}
	}

	void ScriptingSystem::OnEndWorld()
	{
		entt::registry* reg = m_World->m_CurrentRegistry;
		const auto& view = reg->view<ScriptComponent>();
		for (const auto& entity : view)
		{
			auto& component = view.get<ScriptComponent>(entity);

			m_State->m_MetaContext->OnDestroy(&component);
			m_State->m_MonoContext->OnDestroy(&component);
		}

		ClearRuntime();
	}

	void ScriptingSystem::OnUpdate(DeltaTime deltaTime)
	{
		entt::registry* reg = m_World->m_CurrentRegistry;
		const auto& view = reg->view<ScriptComponent>();
		for (const auto& entity : view)
		{
			auto& component = view.get<ScriptComponent>(entity);

			m_State->m_MetaContext->OnUpdate(&component, deltaTime.GetTime());
			m_State->m_MonoContext->OnUpdate(&component);
		}
	}

	void ScriptingSystem::OnDestroy(Actor* actor)
	{
		Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();
		ScriptComponent* script = scene->GetComponent<ScriptComponent>(*actor);
		if (script)
		{
			m_State->m_MetaContext->OnDestroy(script);
			m_State->m_MonoContext->OnDestroy(script);
		}

	}

	void ScriptingSystem::OnCollisionBegin(Actor* actorB, Actor* actorA, bool isTrigger)
	{
		Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();
		ScriptComponent* comp = scene->GetComponent<ScriptComponent>(*actorB);
		if (comp)
		{
			m_State->m_MetaContext->OnCollisionBegin(comp, actorA, isTrigger);
			m_State->m_MonoContext->OnCollisionBegin(comp, actorA, isTrigger);
		}
	}

	void ScriptingSystem::OnCollisionEnd(Actor* actorB, Actor* actorA, bool isTrigger)
	{
		Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();
		ScriptComponent* comp = scene->GetComponent<ScriptComponent>(*actorB);
		if (comp)
		{
			m_State->m_MetaContext->OnCollisionEnd(comp, actorA, isTrigger);
			m_State->m_MonoContext->OnCollisionEnd(comp, actorA, isTrigger);
		}
	}

	void ScriptingSystem::OnConstruct(void* registry_)
	{
		entt::registry* registry = static_cast<entt::registry*>(registry_);
		const auto& view = registry->view<ScriptComponent>();
		for (const auto& entity : view)
		{
			auto& component = view.get<ScriptComponent>(entity);
			m_State->m_MetaContext->OnConstruct(&component);
			m_State->m_MonoContext->OnConstruct(&component);
		}

	}

	void ScriptingSystem::ClearRuntime()
	{
		MonoContext* mono = MonoContext::GetSingleton();
		mono->Shutdown();
		mono->Create();
	}
}