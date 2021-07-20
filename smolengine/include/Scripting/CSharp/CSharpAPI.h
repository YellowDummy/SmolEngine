#pragma once
#include "Core/Core.h"

namespace SmolEngine
{
	struct TransformComponentCSharp;
	struct HeadComponentCSharp;


	// Actor
	bool       GetComponent_CSharpAPI(void* ptr, uint32_t entity_id, uint16_t component_type);
	bool       SetComponent_CSharpAPI(void* ptr, uint32_t entity_id, uint16_t component_type);
	bool       HasComponent_CSharpAPI(uint32_t entity_id, uint16_t component_type);
	bool       AddComponent_CSharpAPI(void* ptr, uint32_t entity_id, uint16_t component_type);
	void*      GetEntityName_CSharpAPI(uint32_t entity_id);
	void*      GetEntityTag_CSharpAPI(uint32_t entity_id);

	// Utils
	bool       IsKeyInput_CSharpAPI(uint16_t key);
	bool       IsMouseInput_CSharpAPI(uint16_t button);
	void       AddMessage_CSharpAPI(void* mono_string, uint32_t level);

	// Mesh
	void       MeshSetVisible_CSharpAPI(uint32_t entity_id, bool value);
	void       MeshResetAll_CSharpAPI(uint32_t entity_id);
	uint32_t   MeshGetChildsCount_CSharpAPI(uint32_t entity_id);
	bool       MeshLoadModel_CSharpAPI(void* str, uint32_t entity_id);
	bool       MeshSetMaterial__CSharpAPI(uint32_t mesh_index, uint32_t material_index, uint32_t entity_id);
	uint32_t   MeshLoadMaterial_CSharpAPI(void* str, uint32_t entity_id);

}