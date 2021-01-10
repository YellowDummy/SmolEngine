#include "stdafx.h"
#include "TransformComponent.h"

namespace SmolEngine
{
	TransformComponent::TransformComponent()
	{

	}

	void TransformComponent::operator=(const TransformComponent& other)
	{
		WorldPos = other.WorldPos;
		Scale = other.Scale;
		Rotation = other.Rotation;
	}
}