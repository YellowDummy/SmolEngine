#pragma once
#include "Core/Core.h"

namespace SmolEngine
{
	class Framebuffer;

	enum class AttachmentFormat: uint16_t
	{
		None,

		Float, 
		Float2, 
		Float3, 
		Float4,

		Int, 
		Int2, 
		Int3, 
		Int4,

		Color
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
				                                   
		bool                                       bTargetsSwapchain = false;
		bool                                       bUsedByImGui = false;
		bool                                       bUseMRT = false;
		bool                                       bUseMSAA = true;
		bool                                       bResizable = true;

		FramebufferAttachment                      ResolveAttachment;
		std::vector<FramebufferAttachment>         Attachments;
	};
}