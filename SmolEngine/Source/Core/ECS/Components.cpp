#include "stdafx.h"

#include "Components.h"
#include "Core/ECS/Actor.h"
#include "Core/Window.h"

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
		if (!Script) 
		{
			NATIVE_ERROR("Invalid Script"); return; 
		}

		Script->OnBeginPlay();
	}

	void ScriptObject::OnDestroy()
	{
		if (!Script) 
		{ 
			NATIVE_ERROR("Invalid Script"); return; 
		}

		Script->OnDestroy();
	}

	void ScriptObject::OnCollisionContact(Actor* actor)
	{
		if (!Script)
		{
			NATIVE_ERROR("Invalid Script"); return;
		}

		Script->OnCollisionContact(actor);
	}

	void ScriptObject::OnCollisionExit(Actor* actor)
	{
		if (!Script)
		{
			NATIVE_ERROR("Invalid Script"); return;
		}

		Script->OnCollisionExit(actor);
	}

	void ScriptObject::OnTriggerContact(Actor* actor)
	{
		if (!Script)
		{
			NATIVE_ERROR("Invalid Script"); return;
		}

		Script->OnTriggerContact(actor);
	}

	void ScriptObject::OnTriggerExit(Actor* actor)
	{
		if (!Script)
		{
			NATIVE_ERROR("Invalid Script"); return;
		}

		Script->OnTriggerExit(actor);
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

	Rigidbody2DComponent::Rigidbody2DComponent(Ref<Actor> actor, Body2DType type)
		:
		Body(std::make_shared<Body2D>(type))
	{
		ActorID = actor->GetID();
	}
}