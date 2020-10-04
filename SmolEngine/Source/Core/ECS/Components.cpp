#include "stdafx.h"

#include "Components.h"
#include "Core/ECS/Actor.h"

namespace SmolEngine
{
	void ScriptObject::OnUpdate(DeltaTime deltaTime)
	{
		//TEMP
		if (Script == nullptr) { NATIVE_ERROR("Invalid Script"); return; }

		Script->OnUpdate(deltaTime);
	}

	void ScriptObject::Start()
	{
		//TEMP
		if (Script == nullptr) { NATIVE_ERROR("Invalid Script"); return; }


		Script->Start();
	}

	void ScriptObject::OnDestroy()
	{
		//TEMP
		if (Script == nullptr) { NATIVE_ERROR("Invalid Script"); return; }

		Script->OnDestroy();
	}

	void TransformComponent::operator=(const TransformComponent& other)
	{
		Rotation = other.Rotation;
		Scale = other.Scale;
		WorldPos = other.WorldPos;
		Enabled = other.Enabled;
	}

	Ref<UIElement> CanvasComponent::GetElement(size_t index)
	{
		if (Canvas)
		{
			return Canvas->GetElement(index);
		}

		return nullptr;
	}

	Ref<UIButton> CanvasComponent::GetButton(size_t index)
	{
		if (Canvas)
		{
			return Canvas->GetButton(index);
		}

		return nullptr;
	}

	Ref<UITextLabel> CanvasComponent::GetTextLabel(size_t index)
	{
		if (Canvas)
		{
			return Canvas->GetTextLabel(index);
		}

		return nullptr;
	}
}