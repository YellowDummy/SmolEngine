#pragma once
#include "Core/Core.h"
#include "Renderer/Shared.h"

namespace SmolEngine
{
	struct BoneInfo
	{
		glm::mat4 FinalTransformation = glm::mat4(0.0f);
		glm::mat4 BoneOffset = glm::mat4(0.0f);
	};

	struct ImportedAnimation
	{

	};

	struct ImportedComponent
	{
		std::string                       Name = "";
		uint32_t                          NumBones = 0;

		std::vector<PBRVertex>            VertexData;
		std::vector<uint32_t>             Indices;
		std::vector<BoneInfo>             BoneInfo;
	};

	struct ImportedData
	{
		std::vector<ImportedComponent>    Components;
		std::vector<ImportedAnimation>    Animations;
		std::map<std::string, uint32_t>   BoneMapping;
	};

	class ModelImporter
	{
	public:

		static bool Load(const std::string& filePath, ImportedData* out_data);
	};
}