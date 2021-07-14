#pragma once
#include "Core/Core.h"

namespace SmolEngine
{
	struct TransformComponentCSharp;
	struct HeadComponentCSharp;

	void GetTransformComponentCSharp(TransformComponentCSharp* obj, uint32_t id);
	void GetHeadComponentCSharp(HeadComponentCSharp* obj, uint32_t id);
}