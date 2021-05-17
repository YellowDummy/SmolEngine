#include "stdafx.h"
#include "ECS/Systems/ScriptingSystem.h"
#include "ECS/Components/BehaviourComponent.h"
#include "ECS/Components/HeadComponent.h"
#include "ECS/Components/Singletons/WorldAdminStateSComponent.h"
#include "ECS/WorldAdmin.h"

#include "Scripting/BehaviourPrimitive.h"

namespace SmolEngine
{
	bool ScriptingSystem::AttachNativeScript(Actor* actor, const std::string& scriptName)
	{
		ScriptingSystemStateSComponent* instance = m_State;
		if (instance == nullptr)
			return false;

		auto& it = instance->MetaMap.find(scriptName);
		if (it == instance->MetaMap.end())
			return false;

		BehaviourComponent* component = nullptr;
		if (!WorldAdmin::GetSingleton()->GetActiveScene()->HasComponent<BehaviourComponent>(actor))
		{
			component = WorldAdmin::GetSingleton()->GetActiveScene()->AddComponent<BehaviourComponent>(actor);
			component->Actor = actor;
			component->ActorID = actor->GetID();
		}
		else
			component = WorldAdmin::GetSingleton()->GetActiveScene()->GetComponent<BehaviourComponent>(actor);

		int32_t index = static_cast<int32_t>(actor->GetComponentsCount());
		actor->GetInfo()->ComponentsCount++;

		BehaviourComponent::ScriptInstance scriptInstance = {};
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
				BehaviourComponent::OutData::IntBuffer intB;
				intB.Name = value.ValueName;
				intB.Value = 0;

				outData.Ints.push_back(intB);
				value.Ptr = &outData.Ints[outData.Ints.size() - 1].Value;
				break;
			}
			case BehaviourPrimitive::OutValueType::Float:
			{
				BehaviourComponent::OutData::FloatBuffer floatB;
				floatB.Name = value.ValueName;
				floatB.Value = 0.0f;

				outData.Floats.push_back(floatB);
				value.Ptr = &outData.Floats[outData.Floats.size() - 1].Value;
				break;
			}
			case BehaviourPrimitive::OutValueType::String:
			{
				BehaviourComponent::OutData::StringBuffer strB;
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

		primitive.m_Actor = actor;
		component->Scripts.emplace_back(scriptInstance);
		return true;
	}

	void ScriptingSystem::OnBegin()
	{
		entt::registry* reg = m_World->m_CurrentRegistry;
		ScriptingSystemStateSComponent* instance = m_State;

		const auto& view = reg->view<BehaviourComponent>();
		for (const auto& entity : view)
		{
			auto& behaviour = view.get<BehaviourComponent>(entity);
			for (auto& script : behaviour.Scripts)
				instance->MetaMap[script.KeyName].OnBeginFunc.invoke(script.Script);
		}
	}

	void ScriptingSystem::OnEnd()
	{
		entt::registry* reg = m_World->m_CurrentRegistry;
		ScriptingSystemStateSComponent* instance = m_State;

		const auto& view = reg->view<BehaviourComponent>();
		for (const auto& entity : view)
		{
			auto& behaviour = view.get<BehaviourComponent>(entity);
			for (auto& script : behaviour.Scripts)
				instance->MetaMap[script.KeyName].OnDestroyFunc.invoke(script.Script);
		}
	}

	void ScriptingSystem::OnTick(Frostium::DeltaTime deltaTime)
	{
		entt::registry* reg = m_World->m_CurrentRegistry;
		ScriptingSystemStateSComponent* instance = m_State;

		const auto& view = reg->view<BehaviourComponent>();
		for (const auto& entity : view)
		{
			auto& behaviour = view.get<BehaviourComponent>(entity);
			for (auto& script : behaviour.Scripts)
				instance->MetaMap[script.KeyName].OnProcessFunc.invoke(script.Script, deltaTime.GetTime());
		}
	}

	void ScriptingSystem::OnDestroy(Actor* actor)
	{
		ScriptingSystemStateSComponent* instance = m_State;

		BehaviourComponent* behaviour = WorldAdmin::GetSingleton()->GetActiveScene()->GetComponent<BehaviourComponent>(actor);
		if (behaviour && instance)
		{
			for (auto& script : behaviour->Scripts)
				instance->MetaMap[script.KeyName].OnDestroyFunc.invoke(script.Script);
		}
	}

	void ScriptingSystem::OnCollisionBegin(Actor* actorB, Actor* actorA, bool isTrigger)
	{
		auto admin = WorldAdmin::GetSingleton();
		if (admin->GetActiveScene()->HasComponent<BehaviourComponent>(*actorB))
		{
			ScriptingSystemStateSComponent* instance = ScriptingSystemStateSComponent::GetSingleton();
			BehaviourComponent* behaviour = admin->GetActiveScene()->GetComponent<BehaviourComponent>(*actorB);

			for (auto& script : behaviour->Scripts)
				instance->MetaMap[script.KeyName].OnCollBeginFunc.invoke(script.Script, actorA, isTrigger);
		}
	}

	void ScriptingSystem::OnCollisionEnd(Actor* actorB, Actor* actorA, bool isTrigger)
	{
		auto admin = WorldAdmin::GetSingleton();
		if (admin->GetActiveScene()->HasComponent<BehaviourComponent>(*actorB))
		{
			ScriptingSystemStateSComponent* instance = ScriptingSystemStateSComponent::GetSingleton();
			BehaviourComponent* behaviour = admin->GetActiveScene()->GetComponent<BehaviourComponent>(*actorB);

			for (auto& script : behaviour->Scripts)
				instance->MetaMap[script.KeyName].OnCollEndFunc.invoke(script.Script, actorA, isTrigger);
		}
	}

	void ScriptingSystem::OnDebugDraw()
	{
		entt::registry* reg = m_World->m_CurrentRegistry;
		ScriptingSystemStateSComponent* instance = m_State;

		const auto& view = reg->view<BehaviourComponent>();
		for (const auto& entity : view)
		{
			auto& behaviour = view.get<BehaviourComponent>(entity);
			for (auto& script : behaviour.Scripts)
				instance->MetaMap[script.KeyName].OnDebugDrawFunc.invoke(script.Script);
		}
	}
}