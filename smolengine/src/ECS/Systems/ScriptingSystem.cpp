#include "stdafx.h"
#include "ECS/Systems/ScriptingSystem.h"
#include "ECS/Components/CppScriptComponent.h"
#include "ECS/Components/HeadComponent.h"
#include "ECS/Components/Singletons/WorldAdminStateSComponent.h"
#include "ECS/WorldAdmin.h"

#include "Scripting/BehaviourPrimitive.h"

namespace SmolEngine
{
	bool ScriptingSystem::AttachNativeScript(Ref<Actor>& actor, const std::string& scriptName)
	{
		ScriptingSystemStateSComponent* instance = m_State;
		if (instance == nullptr)
			return false;

		auto& it = instance->MetaMap.find(scriptName);
		if (it == instance->MetaMap.end())
			return false;

		CppScriptComponent* component = nullptr;
		if (!WorldAdmin::GetSingleton()->GetActiveScene()->HasComponent<CppScriptComponent>(actor))
		{
			component = WorldAdmin::GetSingleton()->GetActiveScene()->AddComponent<CppScriptComponent>(actor);
			component->Actor = actor;
		}
		else
			component = WorldAdmin::GetSingleton()->GetActiveScene()->GetComponent<CppScriptComponent>(actor);

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

	void ScriptingSystem::OnBeginWorld()
	{
		entt::registry* reg = m_World->m_CurrentRegistry;
		ScriptingSystemStateSComponent* instance = m_State;

		const auto& view = reg->view<CppScriptComponent>();
		for (const auto& entity : view)
		{
			auto& behaviour = view.get<CppScriptComponent>(entity);
			for (auto& script : behaviour.Scripts)
				instance->MetaMap[script.KeyName].OnBeginFunc.invoke(script.Script);
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
			for (auto& script : behaviour.Scripts)
				instance->MetaMap[script.KeyName].OnDestroyFunc.invoke(script.Script);
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
			for (auto& script : behaviour.Scripts)
				instance->MetaMap[script.KeyName].OnProcessFunc.invoke(script.Script, deltaTime.GetTime());
		}
	}

	void ScriptingSystem::OnDestroy(Actor* actor)
	{
		ScriptingSystemStateSComponent* instance = m_State;

		CppScriptComponent* behaviour = WorldAdmin::GetSingleton()->GetActiveScene()->GetComponentEX<CppScriptComponent>(actor);
		if (behaviour && instance)
		{
			for (auto& script : behaviour->Scripts)
				instance->MetaMap[script.KeyName].OnDestroyFunc.invoke(script.Script);
		}
	}

	void ScriptingSystem::OnCollisionBegin(Actor* actorB, Actor* actorA, bool isTrigger)
	{
		auto admin = WorldAdmin::GetSingleton();
		if (admin->GetActiveScene()->HasComponent<CppScriptComponent>(*actorB))
		{
			ScriptingSystemStateSComponent* instance = ScriptingSystemStateSComponent::GetSingleton();
			CppScriptComponent* behaviour = admin->GetActiveScene()->GetComponent<CppScriptComponent>(*actorB);

			for (auto& script : behaviour->Scripts)
				instance->MetaMap[script.KeyName].OnCollBeginFunc.invoke(script.Script, actorA, isTrigger);
		}
	}

	void ScriptingSystem::OnCollisionEnd(Actor* actorB, Actor* actorA, bool isTrigger)
	{
		auto admin = WorldAdmin::GetSingleton();
		if (admin->GetActiveScene()->HasComponent<CppScriptComponent>(*actorB))
		{
			ScriptingSystemStateSComponent* instance = ScriptingSystemStateSComponent::GetSingleton();
			CppScriptComponent* behaviour = admin->GetActiveScene()->GetComponent<CppScriptComponent>(*actorB);

			for (auto& script : behaviour->Scripts)
				instance->MetaMap[script.KeyName].OnCollEndFunc.invoke(script.Script, actorA, isTrigger);
		}
	}

	void ScriptingSystem::OnDebugDraw()
	{
		entt::registry* reg = m_World->m_CurrentRegistry;
		ScriptingSystemStateSComponent* instance = m_State;

		const auto& view = reg->view<CppScriptComponent>();
		for (const auto& entity : view)
		{
			auto& behaviour = view.get<CppScriptComponent>(entity);
			for (auto& script : behaviour.Scripts)
				instance->MetaMap[script.KeyName].OnDebugDrawFunc.invoke(script.Script);
		}
	}
}