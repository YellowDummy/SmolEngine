#pragma once

#include "ECS/Actor.h"

#include <string>
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include <cereal/cereal.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>

#include <Frostium3D/Renderer.h>

namespace SmolEngine
{
	struct ScenePipelineState
	{
		RenderingState   m_RendererState;
		SceneState       m_SceneState;

	private:

		friend class cereal::access;
		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(m_RendererState.bBloomPass, m_RendererState.bBlurPass, m_RendererState.bDrawGrid, m_RendererState.bDrawSkyBox,
				m_SceneState.HDRExposure, m_SceneState.UseIBL);
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