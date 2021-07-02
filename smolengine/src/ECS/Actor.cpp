#include "stdafx.h"
#include "ECS/Actor.h"
#include "ECS/WorldAdmin.h"
#include "ECS/Components/HeadComponent.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Scene.h"

namespace SmolEngine
{
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

	uint32_t Actor::GetChildsCount() const
	{
		const HeadComponent* info = GetHead();
		return static_cast<uint32_t>(info->Childs.size());
	}

	Actor* Actor::GetChildByName(const std::string& name)
	{
		HeadComponent* info = GetInfo();
		for (auto& actor : info->Childs)
		{
			if (actor->GetName() == name)
				return actor;
		}

		return nullptr;
	}

	Actor* Actor::GetChildByIndex(uint32_t index)
	{
		HeadComponent* info = GetInfo();
		if (index < info->Childs.size())
			return info->Childs[index];

		return nullptr;
	}

	std::vector<Actor*>& Actor::GetChilds()
	{
		HeadComponent* info = GetInfo();
		return info->Childs;
	}

	uint32_t Actor::GetComponentsCount() const
	{
		return GetHead()->ComponentsCount;
	}

	bool Actor::SetChild(Actor* child)
	{
		if (child != nullptr)
		{
			HeadComponent* p_info = GetInfo();
			uint32_t childID = child->GetID();
			Actor* childNode = FindActorByID(childID);
			bool found = std::find(p_info->Childs.begin(), p_info->Childs.end(), childNode) != p_info->Childs.end();
			if (found == false)
			{
				HeadComponent* c_info = child->GetInfo();
				// Removes old parent
				if (c_info->Parent != nullptr)
				{
					HeadComponent* old_parent_info = c_info->Parent->GetInfo();
					auto& parent_childs = old_parent_info->Childs;
					parent_childs.erase(std::remove(parent_childs.begin(), parent_childs.end(), childNode));
					c_info->Parent = nullptr;
				}
				// Adds new parent
				c_info->ParentID = GetID();
				c_info->Parent = this;
				// Adds new child
				p_info->ChildsIDs.emplace_back(childID);
				p_info->Childs.emplace_back(childNode);
				return true;
			}
		}

		return false;
	}

	bool Actor::SetName(const std::string& name)
	{
		return WorldAdmin::GetSingleton()->ChangeActorName(this, name);
	}

	bool Actor::RemoveChildAtIndex(uint32_t index)
	{
		HeadComponent* info = GetInfo();

		if (index < info->Childs.size())
		{
			Actor* child = info->Childs[index];

			info->Childs.erase(info->Childs.begin() + index);
			info->ChildsIDs.erase(info->ChildsIDs.begin() + index);
			child->GetComponent<TransformComponent>()->RelativePos = glm::vec3(0.0f);
			return true;
		}

		return false;
	}

	HeadComponent* Actor::GetInfo()
	{
		return WorldAdmin::GetSingleton()->GetActiveScene()->GetComponent<HeadComponent>(m_Entity);
	}

	Actor* Actor::FindRootActor(Actor* actor)
	{
		HeadComponent* info = GetInfo();
		if (info->Parent != nullptr)
		{
			Actor* root = info->Parent;
			return FindRootActor(root);
		}

		return actor;
	}

	Actor* Actor::GetParent() const
	{
		return GetHead()->Parent;
	}

	Actor* Actor::GetRootActor()
	{
		return FindRootActor(nullptr);
	}

	Actor* Actor::FindActorByID(uint32_t id)
	{
		return WorldAdmin::GetSingleton()->GetActiveScene()->FindActorByID(id);
	}

	const HeadComponent* Actor::GetHead() const
	{
		return WorldAdmin::GetSingleton()->GetActiveScene()->GetComponent<HeadComponent>(m_Entity);
	}
}