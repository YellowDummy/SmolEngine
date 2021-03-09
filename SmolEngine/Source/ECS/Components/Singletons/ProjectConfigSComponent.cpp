#include "stdafx.h"
#include "ProjectConfigSComponent.h"

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