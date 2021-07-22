#pragma once

#include "ECS/Actor.h"

#include <string>
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include <cereal/cereal.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>

#ifndef FROSTIUM_SMOLENGINE_IMPL
#define FROSTIUM_SMOLENGINE_IMPL
#endif
#include <Frostium3D/DeferredRenderer.h>
#include <Frostium3D/Primitives/CubeMap.h>

namespace SmolEngine
{
	struct SceneStateComponent
	{
		uint32_t                                       SceneID = 0;
		uint32_t                                       LastActorID = 1;
		std::string                                    FilePath = "";
		std::string                                    Name = "";
		std::unordered_map<std::string, Ref<Actor>>    ActorNameSet;
		std::unordered_map<uint32_t, Ref<Actor>>       ActorIDSet;
		std::vector<Ref<Actor>>                        Actors;

	public:

		SceneStateComponent() = default;

	private:

		friend class cereal::access;
		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(SceneID, LastActorID, FilePath, Name, ActorNameSet, ActorIDSet, Actors);
		}
	};
}