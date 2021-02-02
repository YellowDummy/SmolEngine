#pragma once
#include "Core/Core.h"
#include "Renderer/SharedUtils.h"

namespace SmolEngine
{
	struct ImportedData
	{
		std::vector<PBRVertex>            VertexData;
		std::vector<uint32_t>             Indices;
	};

	class ModelImporter
	{
	public:

		static bool Load(const std::string& filePath, ImportedData* out_data);
	};
}