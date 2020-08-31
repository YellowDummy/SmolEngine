#include "stdafx.h"

#include "Components.h"
#include "Core/ECS/Actor.h"

namespace SmolEngine
{
	void ScriptComponent::OnUpdate(DeltaTime deltaTime)
	{
		if (Script == nullptr) { return; }

		Script->OnUpdate(deltaTime);
	}

	void ScriptComponent::Start()
	{
		if (Script == nullptr) { return; }


		Script->Start();
	}

	void ScriptComponent::OnDestroy()
	{
		if (Script == nullptr) { return; }

		Script->OnDestroy();
	}
}