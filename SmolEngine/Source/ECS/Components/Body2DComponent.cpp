#include "stdafx.h"
#include "Body2DComponent.h"

#include "Physics2D/Box2D/Body2D.h"
#include "ECS/Actor.h"

namespace SmolEngine
{
	Body2DComponent::Body2DComponent()
	{

	}

	Body2DComponent::Body2DComponent(uint32 id)
		:BaseComponent(id)
	{
	}

	Body2DComponent::Body2DComponent(Ref<Actor> actor, int type)
	{
		Body.m_Type = type;
		ActorID = actor->GetID();
	}
}