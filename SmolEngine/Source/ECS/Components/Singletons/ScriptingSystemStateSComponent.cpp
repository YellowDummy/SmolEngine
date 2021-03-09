#include "stdafx.h"
#include "ScriptingSystemStateSComponent.h"

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