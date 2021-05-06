#pragma once
#include "Common/Core.h"
#include "Common/Common.h"

#include <unordered_map>
#include <string>

namespace Frostium
{
	enum class BlendFactor: uint16_t
	{
		NONE = 0,
		ONE,
		ZERO,
		SRC_ALPHA,
		SRC_COLOR,
		ONE_MINUS_SRC_COLOR,
		DST_COLOR,
		ONE_MINUS_DST_COLOR,
		ONE_MINUS_SRC_ALPHA,
		DST_ALPHA,
		ONE_MINUS_DST_ALPHA,
		CONSTANT_COLOR,
		ONE_MINUS_CONSTANT_COLOR,
		CONSTANT_ALPHA,
		ONE_MINUS_CONSTANT_ALPHA,
		SRC_ALPHA_SATURATE,
		SRC1_COLOR,
		ONE_MINUS_SRC1_COLOR,
		SRC1_ALPHA,
		ONE_MINUS_SRC1_ALPHA,
	};

	enum class BlendOp: uint16_t
	{
		ADD,
		SUBTRACT,
		REVERSE_SUBTRACT,
		MIN,
		MAX
	};

	struct GraphicsPipelineShaderCreateInfo
	{
		bool                                 Optimize = false;
		bool                                 UseSingleFile = false;

		std::unordered_map<ShaderType,
			std::string>                     FilePaths;
		std::unordered_map<uint32_t,
			size_t>                          StorageBuffersSizes;
		std::string                          SingleFilePath = "";

	};

	enum class DrawMode : uint16_t
	{
		Triangle,
		Line,
		Fan,

		Triangle_Strip,
	};

	enum class PolygonMode : uint16_t
	{
		Fill,
		Line,
		Point
	};

	enum class CullMode : uint16_t
	{
		None,
		Back,
		Front
	};

	enum class PipelineCreateResult : uint16_t
	{
		SUCCESS,
		ERROR_INVALID_CREATE_INFO,
		ERROR_PIPELINE_NOT_INVALIDATED,
		ERROR_PIPELINE_NOT_CREATED,
		ERROR_SHADER_NOT_RELOADED
	};

	struct GraphicsPipelineCreateInfo
	{
		Framebuffer*                         pTargetFramebuffer = nullptr;
		GraphicsPipelineShaderCreateInfo*    pShaderCreateInfo = nullptr;

		BlendFactor                          eSrcColorBlendFactor = BlendFactor::NONE;
		BlendFactor                          eDstColorBlendFactor = BlendFactor::NONE;
		BlendFactor                          eSrcAlphaBlendFactor = BlendFactor::NONE;
		BlendFactor                          eDstAlphaBlendFactor = BlendFactor::NONE;
		BlendOp                              eColorBlendOp = BlendOp::ADD;
		BlendOp                              eAlphaBlendOp = BlendOp::ADD;
		CullMode                             eCullMode = CullMode::Back;
		PolygonMode                          ePolygonMode = PolygonMode::Fill;

		bool                                 bDepthTestEnabled = true;
		bool                                 bDepthWriteEnabled = true;
		bool                                 bDepthBiasEnabled = false;
		float                                MinDepth = 0.0f;
		float                                MaxDepth = 1.0f;
		int32_t                              DescriptorSets = 1;
		int32_t                              StageCount = -1;

		std::string                          PipelineName = "";
		std::vector<DrawMode>                PipelineDrawModes = { DrawMode::Triangle };
		std::vector<VertexInputInfo>         VertexInputInfos;
	};

}
