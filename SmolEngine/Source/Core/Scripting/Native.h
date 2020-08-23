#pragma once
#include "Core/SLog.h"
#include "Core/Core.h"
#include "Core/ECS/ScriptableObject.h"

namespace SmolEngine
{
	class ScriptBase : public ScriptableObject
	{
	public:

		ScriptBase(Ref<Actor> actor)
			:ScriptableObject(actor)
		{

		}

		void Start() override
		{

		}

		void OnUpdate(DeltaTime deltaTime) override
		{
		}

		void OnDestroy() override
		{

		}
	};
}