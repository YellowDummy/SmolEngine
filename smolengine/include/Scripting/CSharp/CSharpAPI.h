#pragma once
#include "Core/Core.h"

namespace SmolEngine
{
	struct TransformComponentCSharp;
	struct HeadComponentCSharp;

	bool  GetComponent_CSharpAPI(void* ptr, uint32_t entity_id, uint16_t component_type);
	bool  SetComponent_CSharpAPI(void* ptr, uint32_t entity_id, uint16_t component_type);
	bool  HasComponent_CSharpAPI(uint32_t entity_id, uint16_t component_type);

	bool  IsKeyInput_CSharpAPI(uint16_t key);
	bool  IsMouseInput_CSharpAPI(uint16_t button);

	void  AddMessage_CSharpAPI(void* mono_string, uint32_t level);
	void* GetEntityName_CSharpAPI(uint32_t entity_id);
	void* GetEntityTag_CSharpAPI(uint32_t entity_id);
}