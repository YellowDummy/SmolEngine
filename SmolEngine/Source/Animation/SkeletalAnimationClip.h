#pragma once
#include "Core/Core.h"
#include "Core/Tools.h"

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace SmolEngine
{
	struct SkeletalAnimationClip
	{
		struct BoneInfo
		{
			glm::mat4                         BoneOffset = glm::mat4(0.0f);
		};

		struct VectorKey
		{
			double      Time;
			glm::vec3   Value;
		};

		struct QuatKey
		{
			double      Time;
			glm::quat   Value;
		};

		bool                              bLoop = false;
		bool                              bDefaultClip = false;

		double                            Duration = 0.0f;
		double                            TicksPerSecond = 0.0f;
		uint32_t                          NumBones = 0;

		std::string                       Name = "";
		std::string                       FilePath = "";
		ToolTimer                         Timer;
		glm::mat4                         FinalTransformation = glm::mat4(0.0f);

		std::vector<VectorKey>            ScaleKeys;
		std::vector<VectorKey>            TransformKeys;
		std::vector<QuatKey>              RotationKeys;
		std::vector<BoneInfo>             BoneInfos;

		std::map<std::string, uint32_t>   BoneMapping;

	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(bLoop, bDefaultClip, FilePath);
		}

	};
}