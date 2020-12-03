#pragma once
#include "Core/Core.h"

namespace SmolEngine
{
	enum class ShaderType : uint32_t
	{
		Vertex,
		Fragment,
		Compute,
		Geometry,

		Invalid
	};
}