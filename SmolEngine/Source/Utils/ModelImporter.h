#pragma once
#include "Core/Core.h"
#include "Renderer/Shared.h"

#include "Animation/SkeletalAnimationClip.h"
#include <assimp/Importer.hpp>

namespace SmolEngine
{
	struct ImportedComponent
	{
		std::string                       Name = "";

		std::vector<PBRVertex>            VertexData;
		std::vector<uint32_t>             Indices;
	};

	struct ImportedData
	{
		SkeletalAnimationClip                            AnimationClip = {};

		uint32_t                                         NumBones = 0;

		std::map<std::string, uint32_t>                  BoneMapping;
		std::vector<SkeletalAnimationClip::BoneInfo>     BoneInfo;
		std::vector<ImportedComponent>                   Components;
	};

	enum class ModelImporterFlags: uint16_t
	{
		Mesh,
		SkeletalAnimation,
		VertexAnimation
	};

	class ModelImporter
	{
	public:

		static bool Load(const std::string& filePath, ImportedData* out_data,
			ModelImporterFlags flags = ModelImporterFlags::Mesh);

		// Helpers

		static glm::vec3 vec3_cast(const aiVector3D& v) { return glm::vec3(v.x, v.y, v.z); }
		static aiVector3D vec3_cast(const glm::vec3& v) { return aiVector3D(v.x, v.y, v.z); }
		static glm::vec2 vec2_cast(const aiVector3D& v) { return glm::vec2(v.x, v.y); } // it's aiVector3D because assimp's texture coordinates use that
		static glm::quat quat_cast(const aiQuaternion& q) { return glm::quat(q.w, q.x, q.y, q.z); }
		static aiQuaternion quat_cast(const glm::quat& q) { return aiQuaternion(q.w, q.x, q.y, q.z); }
	};
}