#include "stdafx.h"
#include "Scripting/BehaviourPrimitive.h"
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

	Actor* BehaviourPrimitive::FindActorByName(const std::string& name)
	{
		return WorldAdmin::GetSingleton()->GetActiveScene()->FindActorByName(name);
	}

	Actor* BehaviourPrimitive::FindActorByTag(const std::string& tag)
	{
		return WorldAdmin::GetSingleton()->GetActiveScene()->FindActorByTag(tag);
	}

	Actor* BehaviourPrimitive::FindActorByID(uint32_t id)
	{
		return WorldAdmin::GetSingleton()->GetActiveScene()->FindActorByID(id);
	}

	void BehaviourPrimitive::GetActors(std::vector<Actor*>& outList)
	{
		return WorldAdmin::GetSingleton()->GetActiveScene()->GetActors(outList);
	}

	void BehaviourPrimitive::GetActorsWithTag(const std::string& tag, std::vector<Actor*>& outList)
	{
		return WorldAdmin::GetSingleton()->GetActiveScene()->GetActorsByTag(tag, outList);
	}
}