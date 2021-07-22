#include "stdafx.h"
#include "Scripting/CPP/MetaContext.h"

#include "ECS/Components/ScriptComponent.h"
#include "Scripting/CPP/BehaviourPrimitive.h"

#include <entt/entt.hpp>

namespace SmolEngine
{
	const std::unordered_map<std::string, MetaContext::MetaData>& MetaContext::GetMeta()
	{
		return m_MetaMap;
	}

	void MetaContext::OnBegin(ScriptComponent* comp)
	{
		OnConstruct(comp);

		for (auto& script : comp->CppScripts)
		{
			m_MetaMap[script.Name].OnBeginFunc.invoke(script.Instance);
		}
	}

	void MetaContext::OnUpdate(ScriptComponent* comp, float deltaTime)
	{
		for (auto& script : comp->CppScripts)
		{
			m_MetaMap[script.Name].OnProcessFunc.invoke(script.Instance, deltaTime);
		}
	}

	void MetaContext::OnDestroy(ScriptComponent* comp)
	{
		for (auto& script : comp->CppScripts)
		{
			m_MetaMap[script.Name].OnDestroyFunc.invoke(script.Instance);
		}
	}

	void MetaContext::OnCollisionBegin(ScriptComponent* comp, Actor* another, bool isTrigger)
	{
		for (auto& script : comp->CppScripts)
		{
			m_MetaMap[script.Name].OnCollBeginFunc.invoke(script.Instance, another, isTrigger);
		}
	}

	void MetaContext::OnCollisionEnd(ScriptComponent* comp, Actor* another, bool isTrigger)
	{
		for (auto& script : comp->CppScripts)
		{
			m_MetaMap[script.Name].OnCollEndFunc.invoke(script.Instance, another, isTrigger);
		}
	}

	void MetaContext::OnConstruct(ScriptComponent* comp)
	{
		if (!comp->pActor)
		{
			NATIVE_ERROR("[MetaContext]: Actor is nullptr!");
			return;
		}

		for (auto& script : comp->CppScripts)
		{
			auto& it = m_MetaMap.find(script.Name);
			if (it == m_MetaMap.end())
			{
				NATIVE_ERROR("[MetaContext]: Script {} not found!", script.Name);
				continue;
			}

			auto& primitive = it->second.ClassInstance.cast<BehaviourPrimitive>();
			if (!script.Fields.AreEqual(&primitive.m_FieldManager))
			{
				script.Fields = primitive.m_FieldManager;
			}
			else
			{
				primitive.m_FieldManager = script.Fields;
			}

			script.Instance = it->second.ClassInstance;
			primitive.m_Actor = comp->pActor;
		}
	}
}