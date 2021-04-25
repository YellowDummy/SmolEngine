#include "stdafx.h"
#include "ECS/Actor.h"
#include "ECS/WorldAdmin.h"
#include "ECS/ComponentsCore.h"

namespace SmolEngine
{
	Actor::Actor(entt::entity entity)
		: m_Entity(entity) {}

	std::vector<Actor*>& Actor::GetChilds()
	{
		return GetInfo()->Childs;
	}

	Actor* Actor::GetChildByName(const std::string& name)
	{
		if (GetHead()->Childs.empty()) { return nullptr; }

		return nullptr;
	}

	Actor* Actor::GetChildByTag(const std::string& tag)
	{
		return nullptr;
	}

	const std::string& Actor::GetName() const
	{
		return GetHead()->Name;
	}

	const std::string& Actor::GetTag() const
	{
		return GetHead()->Tag;
	}

	uint32_t Actor::GetID() const
	{
		return GetHead()->ActorID;
	}

	uint32_t Actor::GetComponentsCount() const
	{
		return GetHead()->ComponentsCount;
	}

	bool Actor::SetParent(Actor* parent)
	{
		return false;
	}

	bool Actor::SetName(const std::string& name)
	{
		return WorldAdmin::GetSingleton()->ChangeActorName(this, name);
	}

	HeadComponent* Actor::GetInfo()
	{
		return WorldAdmin::GetSingleton()->GetActiveScene()->GetComponent<HeadComponent>(m_Entity);
	}

	const HeadComponent* Actor::GetHead() const
	{
		return WorldAdmin::GetSingleton()->GetActiveScene()->GetComponent<HeadComponent>(m_Entity);
	}
}