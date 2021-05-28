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

namespace SmolEngine
{
	struct ScenePipelineState
	{
		int            ImguiSelectable = 0; // internal use
		RendererState  State;

	private:

		friend class cereal::access;
		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(State.bDrawGrid, State.bSSAO, State.bDrawSkyBox, State.bFXAA, State.bHDR, State.eExposureType,
				State.SceneState.HDRExposure, State.SceneState.UseIBL);
		}
	};

	struct SceneStateComponent
	{
		uint32_t                                       SceneID = 0;
		uint32_t                                       LastActorID = 0;
		std::string                                    FilePath = "";
		std::string                                    Name = "";
		ScenePipelineState                             PipelineState = {};
		std::unordered_map<std::string, Actor*>        ActorNameSet;
		std::unordered_map<uint32_t, Actor*>           ActorIDSet;
		std::vector<Actor>                             Actors;

	public:

		SceneStateComponent() = default;

	private:

		friend class cereal::access;
		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(SceneID, LastActorID, FilePath, Name, PipelineState, Actors);
		}
	};
}