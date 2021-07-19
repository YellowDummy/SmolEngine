#include "stdafx.h"
#include "Scripting/CPP/MetaContext.h"

#include "ECS/Components/CppScriptComponent.h"
#include "Scripting/CPP/BehaviourPrimitive.h"

#include <entt/entt.hpp>

namespace SmolEngine
{
	const std::unordered_map<std::string, MetaContext::MetaData>& MetaContext::GetMeta()
	{
		return m_MetaMap;
	}

	void MetaContext::OnBegin(CppScriptComponent* comp)
	{
		for (auto& script : comp->Scripts)
		{
			m_MetaMap[script.KeyName].OnBeginFunc.invoke(script.Script);
		}
	}

	void MetaContext::OnUpdate(CppScriptComponent* comp, float deltaTime)
	{
		for (auto& script : comp->Scripts)
		{
			m_MetaMap[script.KeyName].OnProcessFunc.invoke(script.Script, deltaTime);
		}
	}

	void MetaContext::OnDestroy(CppScriptComponent* comp)
	{
		for (auto& script : comp->Scripts)
		{
			m_MetaMap[script.KeyName].OnDestroyFunc.invoke(script.Script);
		}
	}

	void MetaContext::OnCollisionBegin(CppScriptComponent* comp, Actor* another, bool isTrigger)
	{
		for (auto& script : comp->Scripts)
		{
			m_MetaMap[script.KeyName].OnCollBeginFunc.invoke(script.Script, another, isTrigger);
		}
	}

	void MetaContext::OnCollisionEnd(CppScriptComponent* comp, Actor* another, bool isTrigger)
	{
		for (auto& script : comp->Scripts)
		{
			m_MetaMap[script.KeyName].OnCollEndFunc.invoke(script.Script, another, isTrigger);
		}
	}

	void MetaContext::OnReload(CppScriptComponent* comp)
	{
		if (!comp->Actor)
		{
			NATIVE_ERROR("ScriptingSystem::ReloadScripts::Actor not found!");
			return;
		}

		for (auto& script :comp->Scripts)
		{
			auto& it = m_MetaMap.find(script.KeyName);
			if (it == m_MetaMap.end())
			{
				NATIVE_ERROR("ScriptingSystem::ReloadScripts::Script {} not found!", script.KeyName);
				continue;
			}

			script.Script = it->second.ClassInstance;
			auto& primitive = script.Script.cast<BehaviourPrimitive>();
			primitive.m_Actor = comp->Actor.get();

			auto& s_it = comp->OutValues.find(script.KeyName);
			if (s_it == comp->OutValues.end())
				continue;

			for (auto& valueInt : s_it->second.Ints)
			{
				for (auto& primitiveValue : primitive.m_OutValues)
				{
					if (primitiveValue.Type == BehaviourPrimitive::OutValueType::Int)
					{
						if (valueInt.Name == primitiveValue.ValueName)
							primitiveValue.Ptr = &valueInt.Value;
					}
				}
			}

			for (auto& valueFloat : s_it->second.Floats)
			{
				for (auto& primitiveValue : primitive.m_OutValues)
				{
					if (primitiveValue.Type == BehaviourPrimitive::OutValueType::Float)
					{
						if (valueFloat.Name == primitiveValue.ValueName)
							primitiveValue.Ptr = &valueFloat.Value;
					}
				}
			}

			for (auto& valueString : s_it->second.Strings)
			{
				for (auto& primitiveValue : primitive.m_OutValues)
				{
					if (primitiveValue.Type == BehaviourPrimitive::OutValueType::String)
					{
						if (valueString.Name == primitiveValue.ValueName)
							primitiveValue.Ptr = &valueString.Value;
					}
				}
			}
		}
	}
}