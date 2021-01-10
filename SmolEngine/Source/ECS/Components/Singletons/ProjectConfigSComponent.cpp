#include "stdafx.h"
#include "ProjectConfigSComponent.h"

namespace SmolEngine
{
	ProjectConfigSComponent* ProjectConfigSComponent::Instance = new ProjectConfigSComponent();

	ProjectConfigSComponent::ProjectConfigSComponent()
	{
		Instance = this;
	}

	ProjectConfigSComponent::~ProjectConfigSComponent()
	{
		if (Instance != nullptr)
		{
			Instance =  nullptr;
		}
	}

	ProjectConfigSComponent* ProjectConfigSComponent::Get()
	{
		return Instance;
	}
}