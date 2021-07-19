#include "stdafx.h"

#include "Scripting/CSharp/CSharpAPI.h"
#include "Scripting/CSharp/CSharpDefs.h"
#include "Scripting/CSharp/MonoContext.h"

#include "ECS/ComponentsCore.h"

namespace SmolEngine
{
	bool GetSetTransformComponentCSharp(TransformComponentCSharp* obj, uint32_t id, bool set)
	{
		TransformComponent t1 = {  };
		t1.WorldPos = { 6, 0, 2 };

		obj->WorldPos = t1.WorldPos;
		obj->Scale = t1.Scale;
		obj->Rotation = t1.Rotation;

		return true;
	}

	bool GetSetHeadComponentCSharp(HeadComponentCSharp* obj, uint32_t id, bool set)
	{
		char* mono_name = mono_string_to_utf8(obj->Name);
		char* mono_tag = mono_string_to_utf8(obj->Name);

		std::string name(mono_name);
		std::string tag(mono_tag);

		NATIVE_ERROR("Name: {}, Tag: {}", name, tag);

		mono_free(mono_name);
		mono_free(mono_tag);

		name += "_name";
		tag += "_tag";

		obj->Name = mono_string_new(MonoContext::GetSingleton()->GetDomain(), name.c_str());
		obj->Tag = mono_string_new(MonoContext::GetSingleton()->GetDomain(), tag.c_str());

		return true;
	}
}