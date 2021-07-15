#pragma once
#include "Core/Core.h"

namespace SmolEngine
{
	struct TransformComponentCSharp;
	struct HeadComponentCSharp;

	bool GetSetTransformComponentCSharp(TransformComponentCSharp* obj, uint32_t id, bool set);
	bool GetSetHeadComponentCSharp(HeadComponentCSharp* obj, uint32_t id, bool set);
}