#pragma once
#include "Core/Core.h"

namespace SmolEngine
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

	struct FramebufferAttachment
	{
		FramebufferAttachment() = default;

		FramebufferAttachment(AttachmentFormat _format, bool _bClearOp = false, 
			const std::string& _name = "", bool _alphaBlending = false)
			:Format(_format), bClearOp(_bClearOp), bAlphaBlending(_alphaBlending), Name(_name) { }

		AttachmentFormat                           Format = AttachmentFormat::None;
		std::string                                Name = "";
		bool                                       bClearOp = true;
		bool                                       bAlphaBlending = false;
	};

	struct FramebufferSpecification
	{
		int32_t                                    Width = 0;
		int32_t                                    Height = 0;
		int32_t                                    NumDependencies = 1;
		int32_t                                    NumArrayLayers = 1;
				                                   
		bool                                       bTargetsSwapchain = false;
		bool                                       bUsedByImGui = false;
		bool                                       bUseMSAA = false;
		bool                                       bResizable = true;

		bool                                       bUsingCascadeObject = false;

		FramebufferAttachment                      ResolveAttachment;
		std::vector<FramebufferAttachment>         Attachments;
	};
}