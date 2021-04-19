#include "stdafx.h"
#include "ECS/Components/Singletons/Box2DWorldSComponent.h"

namespace SmolEngine
{
	Box2DWorldSComponent* Box2DWorldSComponent::Instance = nullptr;

	Box2DWorldSComponent::Box2DWorldSComponent()
	{
		Instance = this;
	}

	Box2DWorldSComponent::Box2DWorldSComponent(const float gravityX, const float gravityY)
	{
		World.SetGravity({ gravityX, gravityY });
		Instance = this;
	}

	Box2DWorldSComponent::~Box2DWorldSComponent()
	{
		if (!Instance) { return; }
		Instance = nullptr;
	}
}