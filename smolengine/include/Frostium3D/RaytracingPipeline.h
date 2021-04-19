#pragma once
#include "Common/Core.h"
#include "Common/GraphicsPipelineInfos.h"

#ifndef FROSTIUM_OPENGL_IMPL
#include "Vulkan/VulkanRaytracingPipeline.h"
#endif

namespace Frostium
{
	class Framebuffer;

	struct RaytracingPipelineCreateInfo
	{
		Framebuffer*                       pTargetFramebuffer = nullptr;
		GraphicsPipelineShaderCreateInfo*  pShaderCreateInfo = nullptr;
	};

	class RaytracingPipeline
	{
	public:

		PipelineCreateResult Create(const RaytracingPipelineCreateInfo* info);
	};
}