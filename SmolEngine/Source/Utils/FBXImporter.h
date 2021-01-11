#pragma once
#include "Core/Core.h"

namespace SmolEngine
{
	struct ImportedData
	{
		std::vector<glm::vec4> colors;
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<uint32_t> indices;
	};

	class FBXImporter
	{
	public:

		static bool Load(const std::string& filePath, ImportedData* out_data);
	};
}