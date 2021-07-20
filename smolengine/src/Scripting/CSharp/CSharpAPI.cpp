#include "stdafx.h"

#include "Scripting/CSharp/CSharpAPI.h"
#include "Scripting/CSharp/CSharpDefs.h"
#include "Scripting/CSharp/MonoContext.h"

#include "ECS/ComponentsCore.h"
#include "ECS/ComponentHandler.h"

#include <mono/jit/jit.h>
#include <Frostium3D/Common/Input.h>
#include <Frostium3D/MaterialLibrary.h>

namespace SmolEngine
{
	enum class ComponentTypeEX : uint16_t
	{
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
		Canvas,
		MaxEnum
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
			TransformComponent* native_comp = scene->GetComponent<TransformComponent>(id);
			if (native_comp)
			{
				auto* c_comp = (TransformComponentCSharp*)ptr;
				c_comp->Scale = native_comp->Scale;
				c_comp->WorldPos = native_comp->WorldPos;
				c_comp->Rotation = native_comp->Rotation;
				c_comp->Handler = entity_id;
				return true;
			}

			break;
		}
		case ComponentTypeEX::Mesh:
		{
			MeshComponent* native_comp = scene->GetComponent<MeshComponent>(id);
			if (native_comp)
			{
				auto* c_comp = (MeshComponentCSharp*)ptr;
				c_comp->Handler = entity_id;
				c_comp->IsVisible = native_comp->bShow;
				c_comp->IsActive = native_comp->DefaulPtr != nullptr || native_comp->MeshPtr != nullptr;
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
			TransformComponent* native_comp = scene->GetComponent<TransformComponent>(id);
			if (native_comp)
			{
				TransformComponentCSharp* c_comp = (TransformComponentCSharp*)ptr;
				native_comp->Scale = c_comp->Scale;
				native_comp->WorldPos = c_comp->WorldPos;
				native_comp->Rotation = c_comp->Rotation;
				return true;
			}

			break;
		}
		case ComponentTypeEX::Mesh:
		{
			MeshComponent* native_comp = scene->GetComponent<MeshComponent>(id);
			if (native_comp)
			{
				auto* c_comp = (MeshComponentCSharp*)ptr;
				native_comp->bShow = c_comp->IsVisible;
				return true;
			}

			break;
		}
		}

		return false;
	}

	bool AddComponent_CSharpAPI(void* ptr, uint32_t entity_id, uint16_t component_type)
	{
		return false;
	}

	bool HasComponent_CSharpAPI(uint32_t entity_id, uint16_t component_type)
	{
		ComponentTypeEX type = (ComponentTypeEX)component_type;
		Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();
		entt::entity id = (entt::entity)entity_id;

		switch (type)
		{
		case ComponentTypeEX::Transform: return scene->HasComponent<TransformComponent>(id);
		case ComponentTypeEX::Mesh: return scene->HasComponent<MeshComponent>(id);
		case ComponentTypeEX::RigidBody: return scene->HasComponent<RigidbodyComponent>(id);
		case ComponentTypeEX::RigidBody2D: return scene->HasComponent<Rigidbody2DComponent>(id);
		case ComponentTypeEX::Camera: return scene->HasComponent<CameraComponent>(id);
		case ComponentTypeEX::PointLight: return scene->HasComponent<PointLightComponent>(id);
		case ComponentTypeEX::SpotLight: return scene->HasComponent<SpotLightComponent>(id);
		case ComponentTypeEX::Texture2D: return scene->HasComponent<Texture2DComponent>(id);
		}

		return false;
	}

	void* GetEntityName_CSharpAPI(uint32_t entity_id)
	{
		Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();
		MonoDomain* domain = MonoContext::GetSingleton()->GetDomain();

		HeadComponent* head = scene->GetComponent<HeadComponent>((entt::entity)entity_id);
		MonoString* str = mono_string_new(domain, head->Name.c_str());
		return str;
	}

	void* GetEntityTag_CSharpAPI(uint32_t entity_id)
	{
		Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();
		MonoDomain* domain = MonoContext::GetSingleton()->GetDomain();

		HeadComponent* head = scene->GetComponent<HeadComponent>((entt::entity)entity_id);
		MonoString* str = mono_string_new(domain, head->Tag.c_str());
		return str;
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

	void MeshResetAll_CSharpAPI(uint32_t entity_id)
	{
		Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();
		MeshComponent* comp = scene->GetComponent<MeshComponent>((entt::entity)entity_id);
		comp->bShow = true;
		comp->DefaulPtr = nullptr;
		comp->MeshPtr = nullptr;
		comp->MaterialsData.clear();
		comp->eDefaultType = MeshComponent::DefaultMeshType::None;
		comp->ModelPath = "";
	}

	uint32_t MeshGetChildsCount_CSharpAPI(uint32_t entity_id)
	{
		Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();
		MeshComponent* comp = scene->GetComponent<MeshComponent>((entt::entity)entity_id);
		if (comp)
		{
			if(comp->MeshPtr) { return comp->MeshPtr->GetChildCount(); }
		}

		return 0;
	}

	uint32_t MeshLoadMaterial_CSharpAPI(void* str, uint32_t entity_id)
	{
		Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();
		MeshComponent* comp = scene->GetComponent<MeshComponent>((entt::entity)entity_id);

		if (comp)
		{
			MonoString* mono_str = (MonoString*)str;
			char* cpp_str = mono_string_to_utf8(mono_str);

			MaterialCreateInfo materialCI = {};
			if (materialCI.Load(cpp_str))
			{
				uint32_t material_id = MaterialLibrary::GetSinglenton()->Add(&materialCI, cpp_str);
				mono_free(cpp_str);
				return material_id;
			}

			mono_free(cpp_str);
		}

		return UINT32_MAX;
	}

	bool MeshLoadModel_CSharpAPI(void* str, uint32_t entity_id)
	{
		Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();
		MeshComponent* comp = scene->GetComponent<MeshComponent>((entt::entity)entity_id);

		if (comp)
		{
			MonoString* mono_str = (MonoString*)str;
			char* cpp_str = mono_string_to_utf8(mono_str);
			ComponentHandler::ValidateMeshComponent(comp, cpp_str);

			mono_free(cpp_str);
			return true;
		}

		return false;
	}

	bool MeshSetMaterial__CSharpAPI(uint32_t mesh_index, void* material_path, uint32_t entity_id)
	{
		Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();
		MeshComponent* comp = scene->GetComponent<MeshComponent>((entt::entity)entity_id);

		if (comp)
		{
			if (comp->MeshPtr)
			{
				MonoString* mono_str = (MonoString*)material_path;
				char* cpp_str = mono_string_to_utf8(mono_str);
				Mesh* mesh = &comp->MeshPtr->GetChilds()[mesh_index];

				ComponentHandler::SetMeshMaterial(comp, mesh, cpp_str);
				mono_free(cpp_str);
				return true;
			}
		}

		return false;
	}
}