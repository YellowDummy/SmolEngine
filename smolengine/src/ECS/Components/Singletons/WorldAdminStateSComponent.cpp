#include "stdafx.h"
#include "ECS/Components/Singletons/WorldAdminStateSComponent.h"

namespace SmolEngine
{
	WorldAdminStateSComponent::WorldAdminStateSComponent()
	{
		s_Instance = this;
	}

	WorldAdminStateSComponent::~WorldAdminStateSComponent()
	{
		s_Instance = nullptr;
	}

	WorldAdminStateSComponent::WorldAdminStateSComponent(const WorldAdminStateSComponent& another)
	{
		// dummy
	}
}