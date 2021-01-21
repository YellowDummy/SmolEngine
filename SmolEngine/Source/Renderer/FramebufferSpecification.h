#pragma once
#include "Core/Core.h"

namespace SmolEngine
{
	struct FramebufferSpecification
	{
		int32_t Width = 0;
		int32_t Height = 0;

		bool IsTargetsSwapchain = false;
	};
}