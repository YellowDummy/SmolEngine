#pragma once

#include "Common/Core.h"

#include <string>
#include <vector>

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	class Framebuffer;

	enum class AttachmentFormat: uint16_t
	{
		None,

		UNORM_8,
		UNORM2_8, 
		UNORM3_8, 
		UNORM4_8,

		UNORM_16,
		UNORM2_16,
		UNORM3_16,
		UNORM4_16,

		SFloat_16,
		SFloat2_16,
		SFloat3_16,
		SFloat4_16,

		SFloat_32,
		SFloat2_32,
		SFloat3_32,
		SFloat4_32,

		SInt_8, 
		SInt2_8, 
		SInt3_8, 
		SInt4_8,

		SInt_16,
		SInt2_16,
		SInt3_16,
		SInt4_16,

		SInt_32,
		SInt2_32,
		SInt3_32,
		SInt4_32,

		Color,
		Depth
	};

	enum class FramebufferSpecialisation : uint16_t
	{
		None,
		Raytracing,
		ShadowMap,
	};

	enum class MSAASamples : uint16_t
	{
		SAMPLE_COUNT_1,
		SAMPLE_COUNT_2,
		SAMPLE_COUNT_4,
		SAMPLE_COUNT_8,
		SAMPLE_COUNT_16,

		SAMPLE_COUNT_MAX_SUPPORTED
	};

	enum class SamplerFilter
	{
		LINEAR,
		NEAREST
	};

	struct FramebufferAttachment
	{
		FramebufferAttachment() = default;

		FramebufferAttachment(AttachmentFormat _format, bool _bClearOp = false, 
			const std::string& _name = "")
			:Format(_format), bClearOp(_bClearOp), Name(_name) { }

		AttachmentFormat                           Format = AttachmentFormat::None;
		std::string                                Name = "";
		bool                                       bClearOp = true;
	};

	struct FramebufferSpecification
	{
		MSAASamples                                eMSAASampels = MSAASamples::SAMPLE_COUNT_1;
		FramebufferSpecialisation                  eSpecialisation = FramebufferSpecialisation::None;
		SamplerFilter                              eSamplerFiltering = SamplerFilter::NEAREST;
		bool                                       bTargetsSwapchain = false;
		bool                                       bUsedByImGui = false;
		bool                                       bResizable = true;
		bool                                       bDepthSampler = false;
		int32_t                                    Width = 0;
		int32_t                                    Height = 0;
		int32_t                                    NumSubpassDependencies = 1;
		std::vector<FramebufferAttachment>         Attachments;
	};
}