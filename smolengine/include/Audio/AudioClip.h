#pragma once
#include "Core/Core.h"

#include <string>
#include <glm/glm.hpp>
#include <cereal/cereal.hpp>

namespace SmolEngine
{
	struct AudioClip
	{
		AudioClip();

	public:

		glm::vec3 WorldPos = glm::vec3(0.0f);

		std::string FilePath = "";
		std::string FileName = "";
		std::string ClipName = "";

		float Volume = 1.0f;
		int Channel = 0;

		bool isDefaultClip = false;
		bool IsLooping = false;
		bool B3D = false;
		bool BStream = false;
		bool HasWorldPosition = false;

	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(FilePath, FileName, ClipName, Volume, WorldPos.x,
				WorldPos.y, WorldPos.z, isDefaultClip, IsLooping, B3D,
				BStream, HasWorldPosition, Channel);
		}
	};

}