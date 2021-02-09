#pragma once
#include "Core/Core.h"
#include "Renderer/ShaderTypes.h"

#include <unordered_map>
#include <string>

namespace SmolEngine
{
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
}
