#pragma once
#include "Core/Core.h"
#include "Renderer/Shared.h"

namespace SmolEngine
{
	struct ImportedComponent
	{
		std::vector<PBRVertex>            VertexData;
		std::vector<uint32_t>             Indices;
	};

	struct ImportedData
	{
		std::vector<ImportedComponent>    Components;
	};

	class ModelImporter
	{
	public:

		static bool Load(const std::string& filePath, ImportedData* out_data);
	};
}