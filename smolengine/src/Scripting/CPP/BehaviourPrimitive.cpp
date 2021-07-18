#include "stdafx.h"
#include "Scripting/CPP/BehaviourPrimitive.h"
#include "ECS/Scene.h"


namespace SmolEngine
{
	const std::string& BehaviourPrimitive::GetName()
	{
		return m_Actor->GetName();
	}

	const std::string& BehaviourPrimitive::GetTag()
	{
		return m_Actor->GetTag();
	}

	const size_t BehaviourPrimitive::GetID()
	{
		return m_Actor->GetID();
	}

	Ref<Actor> BehaviourPrimitive::FindActorByName(const std::string& name)
	{
		return WorldAdmin::GetSingleton()->GetActiveScene()->FindActorByName(name);
	}

	Ref<Actor> BehaviourPrimitive::FindActorByTag(const std::string& tag)
	{
		return WorldAdmin::GetSingleton()->GetActiveScene()->FindActorByTag(tag);
	}

	Ref<Actor> BehaviourPrimitive::FindActorByID(uint32_t id)
	{
		return WorldAdmin::GetSingleton()->GetActiveScene()->FindActorByID(id);
	}

	void BehaviourPrimitive::GetActors(std::vector<Ref<Actor>>& outList)
	{
		return WorldAdmin::GetSingleton()->GetActiveScene()->GetActors(outList);
	}

	void BehaviourPrimitive::GetActorsWithTag(const std::string& tag, std::vector<Ref<Actor>>& outList)
	{
		return WorldAdmin::GetSingleton()->GetActiveScene()->GetActorsByTag(tag, outList);
	}

	uint32_t BehaviourPrimitive::GetChildsCount() const
	{
		return m_Actor->GetChildsCount();
	}

	Ref<Actor> BehaviourPrimitive::GetChildByName(const std::string& name)
	{
		return m_Actor->GetChildByName(name);
	}

	Ref<Actor> BehaviourPrimitive::GetChildByIndex(uint32_t index)
	{
		return m_Actor->GetChildByIndex(index);
	}

	std::vector<Ref<Actor>>& BehaviourPrimitive::GetChilds()
	{
		return m_Actor->GetChilds();
	}

	Ref<Actor> BehaviourPrimitive::GetParent() const
	{
		return m_Actor->GetParent();
	}
}