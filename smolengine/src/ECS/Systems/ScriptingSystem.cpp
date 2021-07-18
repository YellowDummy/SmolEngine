#include "stdafx.h"
#include "ECS/Systems/ScriptingSystem.h"
#include "ECS/Components/CppScriptComponent.h"
#include "ECS/Components/CSharpScriptComponent.h"
#include "ECS/Components/HeadComponent.h"
#include "ECS/Components/Singletons/WorldAdminStateSComponent.h"
#include "ECS/WorldAdmin.h"

#include "Scripting/CPP/BehaviourPrimitive.h"
#include "Scripting/CPP/MetaContext.h"
#include "Scripting/CSharp/MonoContext.h"

namespace SmolEngine
{
	bool ScriptingSystem::AttachNativeScript(Ref<Actor>& actor, const std::string& scriptName)
	{
		MetaContext* meta_context = m_State->m_MetaContext;
		auto& it = meta_context->m_MetaMap.find(scriptName);
		if (it == meta_context->m_MetaMap.end())
			return false;

		CppScriptComponent* component = GetOrCreateComponent<CppScriptComponent>(actor);
		component->Actor = actor;

		int32_t index = static_cast<int32_t>(actor->GetComponentsCount());
		actor->GetInfo()->ComponentsCount++;

		CppScriptComponent::ScriptInstance scriptInstance = {};
		scriptInstance.KeyName = scriptName;
		scriptInstance.Script = it->second.ClassInstance;

		auto& primitive = scriptInstance.Script.cast<BehaviourPrimitive>();
		auto& outData = component->OutValues[scriptName];
		outData.ScriptID = index;

		for (auto& value : primitive.m_OutValues)
		{
			switch (value.Type)
			{
			case BehaviourPrimitive::OutValueType::Int:
			{
				CppScriptComponent::OutData::IntBuffer intB;
				intB.Name = value.ValueName;
				intB.Value = 0;

				outData.Ints.push_back(intB);
				value.Ptr = &outData.Ints[outData.Ints.size() - 1].Value;
				break;
			}
			case BehaviourPrimitive::OutValueType::Float:
			{
				CppScriptComponent::OutData::FloatBuffer floatB;
				floatB.Name = value.ValueName;
				floatB.Value = 0.0f;

				outData.Floats.push_back(floatB);
				value.Ptr = &outData.Floats[outData.Floats.size() - 1].Value;
				break;
			}
			case BehaviourPrimitive::OutValueType::String:
			{
				CppScriptComponent::OutData::StringBuffer strB;
				strB.Name = value.ValueName;
				strB.Value = "DefaultName";

				outData.Strings.push_back(strB);
				value.Ptr = &outData.Strings[outData.Strings.size() - 1].Value;
				break;
			}
			default:
				break;
			}
		}

		primitive.m_Actor = actor.get();
		component->Scripts.emplace_back(scriptInstance);
		return true;
	}

	bool ScriptingSystem::AttachCSharpScript(Ref<Actor>& actor, const std::string& className)
	{
		MonoContext* mono = m_State->m_MonoContext;
		bool class_found = std::find(mono->m_ClassNames.begin(), mono->m_ClassNames.end(), className) != mono->m_ClassNames.end();
		if (class_found)
		{
			CSharpScriptComponent* component = GetOrCreateComponent<CSharpScriptComponent>(actor);
			Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();

			return true;
		}



		return false;
	}

	void ScriptingSystem::OnBeginWorld()
	{
		entt::registry* reg = m_World->m_CurrentRegistry;
		ScriptingSystemStateSComponent* instance = m_State;

		const auto& view = reg->view<CppScriptComponent>();
		for (const auto& entity : view)
		{
			auto& behaviour = view.get<CppScriptComponent>(entity);
			m_State->m_MetaContext->OnBegin(&behaviour);
		}
	}

	void ScriptingSystem::OnEndWorld()
	{
		entt::registry* reg = m_World->m_CurrentRegistry;
		ScriptingSystemStateSComponent* instance = m_State;

		const auto& view = reg->view<CppScriptComponent>();
		for (const auto& entity : view)
		{
			auto& behaviour = view.get<CppScriptComponent>(entity);
			m_State->m_MetaContext->OnDestroy(&behaviour);
		}
	}

	void ScriptingSystem::OnUpdate(DeltaTime deltaTime)
	{
		entt::registry* reg = m_World->m_CurrentRegistry;
		ScriptingSystemStateSComponent* instance = m_State;

		const auto& view = reg->view<CppScriptComponent>();
		for (const auto& entity : view)
		{
			auto& behaviour = view.get<CppScriptComponent>(entity);
			m_State->m_MetaContext->OnUpdate(&behaviour, deltaTime.GetTime());
		}
	}

	void ScriptingSystem::OnDestroy(Actor* actor)
	{
		Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();
		CppScriptComponent* behaviour = scene->GetComponentEX<CppScriptComponent>(actor);
		if (behaviour)
			m_State->m_MetaContext->OnDestroy(behaviour);
	}

	void ScriptingSystem::OnCollisionBegin(Actor* actorB, Actor* actorA, bool isTrigger)
	{
		Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();
		if (scene->HasComponent<CppScriptComponent>(*actorB))
		{
			CppScriptComponent* comp = scene->GetComponent<CppScriptComponent>(*actorB);
			m_State->m_MetaContext->OnCollisionBegin(comp, actorA, isTrigger);
		}
	}

	void ScriptingSystem::OnCollisionEnd(Actor* actorB, Actor* actorA, bool isTrigger)
	{
		ScriptingSystemStateSComponent* instance = ScriptingSystemStateSComponent::GetSingleton();
		Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();
		if (scene->HasComponent<CppScriptComponent>(*actorB))
		{
			CppScriptComponent* comp = scene->GetComponent<CppScriptComponent>(*actorB);
			instance->m_MetaContext->OnCollisionEnd(comp, actorA, isTrigger);
		}
	}

	void ScriptingSystem::OnSceneReloaded(void* registry_)
	{
		m_State->m_MetaContext->OnSceneReloaded(registry_);
	}
}