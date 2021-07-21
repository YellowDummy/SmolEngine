#pragma once
#include "Core/Core.h"

namespace SmolEngine
{
	struct TransformComponentCSharp;
	struct HeadComponentCSharp;
	struct RigidBodyCreateInfoCSharp;


	// Actor
	bool       GetComponent_CSharpAPI(void* ptr, uint32_t entity_id, uint16_t component_type);
	bool       SetComponent_CSharpAPI(void* ptr, uint32_t entity_id, uint16_t component_type);
	bool       HasComponent_CSharpAPI(uint32_t entity_id, uint16_t component_type);
	bool       AddComponent_CSharpAPI(void* ptr, uint32_t entity_id, uint16_t component_type);
	bool       DestroyComponent_CSharpAPI(uint32_t entity_id, uint16_t component_type);
	void*      GetEntityName_CSharpAPI(uint32_t entity_id);
	void*      GetEntityTag_CSharpAPI(uint32_t entity_id);

	// Utils
	bool       IsKeyInput_CSharpAPI(uint16_t key);
	bool       IsMouseInput_CSharpAPI(uint16_t button);
	void       AddMessage_CSharpAPI(void* mono_string, uint32_t level);
	void       GetMousePos_CSharpAPI(glm::vec2* pos);

	// Mesh
	void       MeshResetAll_CSharpAPI(uint32_t entity_id);
	uint32_t   MeshGetChildsCount_CSharpAPI(uint32_t entity_id);
	bool       MeshLoadModel_CSharpAPI(void* str, uint32_t entity_id);
	bool       MeshSetMaterial__CSharpAPI(uint32_t mesh_index, void* material_path, uint32_t entity_id);
	uint32_t   MeshLoadMaterial_CSharpAPI(void* str, uint32_t entity_id);

	// Rigidbody
	void      RigidBodyCreate_CSharpAPI(RigidBodyCreateInfoCSharp* ptr, uint32_t entity_id);
	void      RigidBodySetImpact_CSharpAPI(glm::vec3* dir, uint32_t entity_id, uint16_t flags);

}