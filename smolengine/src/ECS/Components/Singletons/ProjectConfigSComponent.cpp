#include "stdafx.h"
#include "ECS/Components/Singletons/ProjectConfigSComponent.h"

namespace SmolEngine
{
	ProjectConfigSComponent::ProjectConfigSComponent()
	{
		Instance = this;
	}

	ProjectConfigSComponent::~ProjectConfigSComponent()
	{
		Instance = nullptr;
	}

	ProjectConfigSComponent* ProjectConfigSComponent::Get()
	{
		return Instance;
	}
}