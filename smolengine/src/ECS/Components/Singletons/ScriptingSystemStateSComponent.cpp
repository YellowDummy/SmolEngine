#include "stdafx.h"
#include "ECS/Components/Singletons/ScriptingSystemStateSComponent.h"

namespace SmolEngine
{
	ScriptingSystemStateSComponent::ScriptingSystemStateSComponent()
	{
		Instance = this;
	}

	ScriptingSystemStateSComponent::~ScriptingSystemStateSComponent()
	{
		Instance = nullptr;
	}

	ScriptingSystemStateSComponent* ScriptingSystemStateSComponent::GetSingleton()
	{
		return Instance;
	}
}