#include "stdafx.h"
#include "ECS/Components/Singletons/GraphicsEngineSComponent.h"

namespace SmolEngine
{
	GraphicsEngineSComponent::GraphicsEngineSComponent()
	{
		Instance = this;
	}

	GraphicsEngineSComponent::~GraphicsEngineSComponent()
	{
		Instance = nullptr;
	}
}
