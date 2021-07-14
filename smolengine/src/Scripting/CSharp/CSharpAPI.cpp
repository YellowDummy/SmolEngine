#include "stdafx.h"
#include "Scripting/CSharp/CSharpAPI.h"
#include "Scripting/CSharp/CSharpDefs.h"

#include "ECS/ComponentsCore.h"

namespace SmolEngine
{
	Vector3 FromGLM(const glm::vec3& vec)
	{
		Vector3 result;
		result.X = vec.x;
		result.Y = vec.y;
		result.Z = vec.z;

		return result;
	}

	void GetTransformComponentCSharp(TransformComponentCSharp* obj, uint32_t id)
	{
		TransformComponent t1 = {  };
		t1.WorldPos = { 2, 2, 2 };

		obj->WorldPos = FromGLM(t1.WorldPos);
		obj->Scale = FromGLM(t1.Scale);
		obj->Rotation = FromGLM(t1.Rotation);
	}

	void GetHeadComponentCSharp(HeadComponentCSharp* obj, uint32_t id)
	{

	}
}