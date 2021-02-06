#pragma once
#include "Core/Core.h"

namespace SmolEngine
{
	class Framebuffer;

	struct FramebufferSpecification
	{
		int32_t    Width = 0;
		int32_t    Height = 0;

		bool       IsTargetsSwapchain = false;
		bool       IsUseMRT = false;
		bool       IsUseMSAA = true;

	};
}