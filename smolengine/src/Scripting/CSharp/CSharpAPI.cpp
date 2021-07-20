#include "stdafx.h"

#include "Scripting/CSharp/CSharpAPI.h"
#include "Scripting/CSharp/CSharpDefs.h"
#include "Scripting/CSharp/MonoContext.h"

#include "ECS/ComponentsCore.h"

#include <mono/jit/jit.h>
#include <Frostium3D/Common/Input.h>

namespace SmolEngine
{
	enum class ComponentTypeEX : uint16_t
	{
		Null,
		Transform,
		Camera,
		RigidBody,
		RigidBody2D,
		Mesh,
		PointLight,
		SpotLight,
		Light2D,
		Texture2D,
		RendererState,
		Canvas
	};

	bool GetComponent_CSharpAPI(void* ptr, uint32_t entity_id, uint16_t component_type)
	{
		ComponentTypeEX type = (ComponentTypeEX)component_type;
		Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();
		entt::entity id = (entt::entity)entity_id;

		switch (type)
		{
		case ComponentTypeEX::Transform:
		{
			TransformComponent* native_component = scene->GetComponent<TransformComponent>(id);
			if (native_component)
			{
				TransformComponentCSharp* c_component = (TransformComponentCSharp*)ptr;

				c_component->Scale = native_component->Scale;
				c_component->WorldPos = native_component->WorldPos;
				c_component->Rotation = native_component->Rotation;

				return true;
			}

			break;
		}
		}

		return false;
	}

	bool SetComponent_CSharpAPI(void* ptr, uint32_t entity_id, uint16_t component_type)
	{
		ComponentTypeEX type = (ComponentTypeEX)component_type;
		Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();
		entt::entity id = (entt::entity)entity_id;

		switch (type)
		{
		case ComponentTypeEX::Transform:
		{
			TransformComponent* native_component = scene->GetComponent<TransformComponent>(id);
			if (native_component)
			{
				TransformComponentCSharp* c_component = (TransformComponentCSharp*)ptr;

				native_component->Scale = c_component->Scale;
				native_component->WorldPos = c_component->WorldPos;
				native_component->Rotation = c_component->Rotation;
				return true;
			}

			break;
		}
		}

		return false;
	}

	bool HasComponent_CSharpAPI(uint32_t entity_id, uint16_t component_type)
	{
		return false;
	}

	bool IsKeyInput_CSharpAPI(uint16_t key)
	{
		return Input::IsKeyPressed((KeyCode)key);
	}

	bool IsMouseInput_CSharpAPI(uint16_t button)
	{
		return Input::IsMouseButtonPressed((MouseCode)button);
	}

	void AddMessage_CSharpAPI(void* mono_string, uint32_t level)
	{
		MonoString* mono_str = (MonoString*)mono_string;
		char* cpp_str = mono_string_to_utf8(mono_str);

		switch (level)
		{
		case 0: NATIVE_INFO(cpp_str); break;
		case 1: NATIVE_WARN(cpp_str); break;
		case 2: NATIVE_ERROR(cpp_str); break;
		}

		mono_free(cpp_str);
	}

	void* GetEntityName_CSharpAPI(uint32_t entity_id)
	{
		return nullptr;
	}

	void* GetEntityTag_CSharpAPI(uint32_t entity_id)
	{
		return nullptr;
	}
}