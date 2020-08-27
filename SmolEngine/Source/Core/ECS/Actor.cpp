#include "stdafx.h"
#include "Actor.h"
#include "Core/ECS/Scene.h"

namespace SmolEngine
{
	Actor::Actor(const entt::entity& entity, entt::registry& reg, const std::string& name, const std::string& tag, const size_t id)
		:Parent(nullptr),
		Entity(entity),
		Tag(tag),
		Reg(reg),
		Name(name),
		IsDisabled(false)
	{
		ID = id;
	}

	Ref<Actor> Actor::operator==(const Ref<Actor> other)
	{
		if (other->GetID() == ID)
		{
			return other;
		}

		return nullptr;;
	}

	Ref<Actor> Actor::GetChildByName(const std::string& name)
	{
		if (Childs.empty()) { return nullptr; }
		auto set = Scene::GetScene()->GetIDSet();
		size_t id = set[name];

		for (auto child : Childs)
		{
			if (child->GetID() == id)
			{
				return child;
			}
		}

		return nullptr;
	}

	Ref<Actor> Actor::GetChildByTag(const std::string& tag)
	{
		if (Childs.empty()) { return nullptr; }
		for (auto child : Childs)
		{
			if (child->GetTag() == tag)
			{
				return child;
			}
		}

		return nullptr;
	}
}