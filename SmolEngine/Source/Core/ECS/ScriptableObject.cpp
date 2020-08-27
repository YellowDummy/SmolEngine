#include "stdafx.h"
#include "Core/ECS/ScriptableObject.h"
#include "Core/ECS/Actor.h"
#include "Core/ECS/Scene.h"

namespace SmolEngine
{
	std::string& ScriptableObject::GetName()
	{
		return m_Actor->GetName();
	}

	const std::string& ScriptableObject::GetTag()
	{
		return m_Actor->GetTag();
	}

	Ref<Actor> ScriptableObject::GetParent()
	{
		return m_Actor->GetParent();
	}

	void ScriptableObject::SetParent(Ref<Actor> parent)
	{
		m_Actor->SetParent(parent);
	}

	std::vector<Ref<Actor>>& ScriptableObject::GetChilds()
	{
		return m_Actor->GetChilds();
	}

	std::vector<Ref<Actor>>& ScriptableObject::GetActorList()
	{
		return Scene::GetScene()->GetActorPool();
	}

	std::vector<Ref<Actor>> ScriptableObject::GetActorListByTag(const std::string& tag)
	{
		return Scene::GetScene()->GetActorListByTag(tag);
	}

	void ScriptableObject::AddChild(Ref<Actor> child)
	{
		Scene::GetScene()->AddChild(m_Actor, child);
	}

	void ScriptableObject::RemoveChild(Ref<Actor> child)
	{
		Scene::GetScene()->RemoveChild(m_Actor, child);
	}

	Ref<Actor> ScriptableObject::GetChildByName(const std::string& name)
	{
		return m_Actor->GetChildByName(name);
	}

	Ref<Actor> ScriptableObject::GetChildByTag(const std::string& tag)
	{
		return m_Actor->GetChildByTag(tag);
	}

	Ref<Actor> ScriptableObject::GetActorByName(const std::string& name)
	{
		return Scene::GetScene()->FindActorByName(name);
	}

	Ref<Actor> ScriptableObject::GetActorByTag(const std::string& tag)
	{
		return Scene::GetScene()->FindActorByTag(tag);
	}
}