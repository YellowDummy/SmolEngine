#include "stdafx.h"

#include "Scripting/CSharp/CSharpAPI.h"
#include "Scripting/CSharp/CSharpDefs.h"
#include "Scripting/CSharp/MonoContext.h"

#include "ECS/ComponentsCore.h"
#include "ECS/ComponentHandler.h"
#include "ECS/Systems/PhysicsSystem.h"

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

	enum class ImpactFlags : uint16_t
	{
		Force,
		Impulse,
		Torque,
		Gravity
	};

	void SetGetTransform(TransformComponent* native_comp, TransformComponentCSharp* c_comp, uint32_t entity_id, bool get)
	{
		if (get)
		{
			c_comp->Scale = native_comp->Scale;
			c_comp->WorldPos = native_comp->WorldPos;
			c_comp->Rotation = native_comp->Rotation;
			c_comp->Handler = entity_id;
		}
		else
		{
			native_comp->Scale = c_comp->Scale;
			native_comp->WorldPos = c_comp->WorldPos;
			native_comp->Rotation = c_comp->Rotation;
		}
	}

	void SetGetMesh_Component(MeshComponent* native_comp, MeshComponentCSharp* c_comp, uint32_t entity_id, bool get)
	{
		if (get)
		{
			c_comp->Handler = entity_id;
			c_comp->IsVisible = native_comp->bShow;
			c_comp->IsActive = native_comp->DefaulPtr != nullptr || native_comp->MeshPtr != nullptr;
		}
		else
		{
			native_comp->bShow = c_comp->IsVisible;
		}
	}

	bool ModifyComponent(void* ptr, uint32_t entity_id, uint16_t component_type, bool get_flag, bool add_flag = false)
	{
		ComponentTypeEX type = (ComponentTypeEX)component_type;
		Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();
		entt::entity id = (entt::entity)entity_id;

		switch (type)
		{
		case ComponentTypeEX::Transform:
		{
			auto* native_comp = add_flag == false ? scene->GetComponent<TransformComponent>(id): scene->AddComponent<TransformComponent>(id);
			auto* c_comp = (TransformComponentCSharp*)ptr;
			if (native_comp)
				SetGetTransform(native_comp, c_comp, entity_id, get_flag);

			return native_comp != nullptr;
		}
		case ComponentTypeEX::Mesh:
		{
			auto* native_comp = add_flag == false ?  scene->GetComponent<MeshComponent>(id): scene->AddComponent<MeshComponent>(id);
			auto* c_comp = (MeshComponentCSharp*)ptr;
			if (native_comp)
				SetGetMesh_Component(native_comp, c_comp, entity_id, get_flag);

			return native_comp != nullptr;
		}
		case ComponentTypeEX::RigidBody:
		{
			auto* native_comp = add_flag == false ? scene->GetComponent<RigidbodyComponent>(id) : scene->AddComponent<RigidbodyComponent>(id);
			auto* c_comp = (RigidBodyComponentCSharp*)ptr;
			c_comp->Handler = entity_id;
			return native_comp != nullptr;
		}
		}

		return false;
	}

	bool GetComponent_CSharpAPI(void* ptr, uint32_t entity_id, uint16_t component_type)
	{
		const bool get_flag = true;
		return ModifyComponent(ptr, entity_id, component_type, get_flag);
	}

	bool SetComponent_CSharpAPI(void* ptr, uint32_t entity_id, uint16_t component_type)
	{
		const bool get_flag = true;
		return ModifyComponent(ptr, entity_id, component_type, get_flag);
	}

	bool AddComponent_CSharpAPI(void* ptr, uint32_t entity_id, uint16_t component_type)
	{
		const bool get_flag = true;
		const bool add_flag = true;
		return ModifyComponent(ptr, entity_id, component_type, get_flag, add_flag);
	}

	bool DestroyComponent_CSharpAPI(uint32_t entity_id, uint16_t component_type)
	{
		ComponentTypeEX type = (ComponentTypeEX)component_type;
		Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();
		entt::entity id = (entt::entity)entity_id;

		switch (type)
		{
		case ComponentTypeEX::Transform: return scene->DestroyComponent<TransformComponent>(id);
		case ComponentTypeEX::Mesh: return scene->DestroyComponent<MeshComponent>(id);
		case ComponentTypeEX::RigidBody: return scene->DestroyComponent<RigidbodyComponent>(id);
		case ComponentTypeEX::RigidBody2D: return scene->DestroyComponent<Rigidbody2DComponent>(id);
		case ComponentTypeEX::Camera: return scene->DestroyComponent<CameraComponent>(id);
		case ComponentTypeEX::PointLight: return scene->DestroyComponent<PointLightComponent>(id);
		case ComponentTypeEX::SpotLight: return scene->DestroyComponent<SpotLightComponent>(id);
		case ComponentTypeEX::Texture2D: return scene->DestroyComponent<Texture2DComponent>(id);
		}

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

	void RigidBodyCreate_CSharpAPI(RigidBodyCreateInfoCSharp* ptr, uint32_t entity_id)
	{
		Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();
		entt::entity id = (entt::entity)entity_id;

		HeadComponent* head = scene->GetComponent<HeadComponent>(id);
		Ref<Actor> actor = scene->FindActorByID(head->ActorID);
		RigidbodyComponent* component = scene->GetComponent<RigidbodyComponent>(id);
		if (!component) { component = scene->AddComponent<RigidbodyComponent>(id); }

		BodyCreateInfo info = {};
		info.eShape = (RigidBodyShape)ptr->eShape;
		info.eType = (RigidBodyType)ptr->eType;
		info.Size = ptr->Size;
		info.Mass = ptr->Mass;
		info.Density = ptr->Density;
		info.Friction = ptr->Friction;
		info.Restitution = ptr->Restitution;
		info.LinearDamping = ptr->LinearDamping;
		info.AngularDamping = ptr->AngularDamping;
		info.RollingFriction = ptr->RollingFriction;
		info.SpinningFriction = ptr->SpinningFriction;

		component->CreateInfo = info;
		ComponentHandler::ValidateRigidBodyComponent(component, actor);
	}

	void RigidBodySetImpact_CSharpAPI(glm::vec3* value, uint32_t entity_id, uint16_t flags)
	{
		ImpactFlags flag_ = (ImpactFlags)flags;
		Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();
		RigidbodyComponent* rb = scene->GetComponent<RigidbodyComponent>((entt::entity)entity_id);
		if (rb)
		{
			switch (flag_)
			{
			case ImpactFlags::Force: PhysicsSystem::AddForce(rb, *value); break;
			case ImpactFlags::Impulse: PhysicsSystem::AddImpulse(rb, *value); break;
			case ImpactFlags::Torque: PhysicsSystem::AddTorque(rb, *value); break;
			case ImpactFlags::Gravity: PhysicsSystem::SetGravity(rb, *value); break;
			}
		}
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