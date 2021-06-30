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
#include <Frostium3D/Common/CubeMap.h>

namespace SmolEngine
{
	struct Mask
	{
		Ref<Texture> Texture = nullptr;
		std::string  Path = "";
		float        Intensity = 1.0f;
		float        IntensityBase = 0.1f;

	private:
		bool         bReset = false;

		friend class cereal::access;
		friend class RendererPanel;
		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Path, Intensity, IntensityBase);
		}
	};

	struct EnvironmentData
	{
		// static cube map
		std::string          CubeMapPath = "";
		CubeMap*             CubeMap = nullptr;
		TextureFormat        eFormat = TextureFormat::B8G8R8A8_UNORM;
		// dynamic cube map
		bool                 bGeneratePBRMaps = true;
		DynamicSkyProperties SkyProperties = {};

	private:
		friend class cereal::access;
		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(CubeMapPath, eFormat, bGeneratePBRMaps, SkyProperties.AtmosphereRadius, SkyProperties.MieScale, SkyProperties.MieScatteringCoeff, SkyProperties.MieScatteringDirection,
				SkyProperties.PlanetRadius, SkyProperties.RayleighScale, SkyProperties.RayleighScatteringCoeff.x, SkyProperties.RayleighScatteringCoeff.y, SkyProperties.RayleighScatteringCoeff.z,
				SkyProperties.RayOrigin.x, SkyProperties.RayOrigin.y, SkyProperties.RayOrigin.z, SkyProperties.SunIntensity, SkyProperties.SunPosition.x, SkyProperties.SunPosition.y, SkyProperties.SunPosition.z,
				SkyProperties.NumCirrusCloudsIterations, SkyProperties.NumCumulusCloudsIterations);
		}
	};

	struct ScenePipelineState
	{
		// internal use
		int                  PostProcessingFlag = 0;
		int                  DebugViewFlag = 0;
		int                  EnvironmentFlags = 0;
		int                  FormatFlags = 0;
	   
		RendererState        State = {};
		EnvironmentData      Environment = {};
		Mask                 DirtMask = {};

	private:

		friend class cereal::access;
		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(State.bDrawGrid, State.bIBL, State.bSSAO, State.bDrawSkyBox, State.bFXAA, State.bBloom,
				State.Lighting.AmbientColor.r, State.Lighting.AmbientColor.g, State.Lighting.AmbientColor.b,
				State.Lighting.IBLStrength, State.Bloom.Exposure, State.Bloom.Scale, State.Bloom.Strength,
				State.Bloom.Threshold, State.FXAA.EdgeThresholdMax, State.FXAA.EdgeThresholdMin, State.FXAA.Iterations, State.FXAA.SubPixelQuality, 
				Environment, DirtMask);
		}
	};

	struct SceneStateComponent
	{
		uint32_t                                       SceneID = 0;
		uint32_t                                       LastActorID = 1;
		std::string                                    FilePath = "";
		std::string                                    Name = "";
		ScenePipelineState                             PipelineState = {};
		std::unordered_map<std::string, Actor*>        ActorNameSet;
		std::unordered_map<uint32_t, Actor*>           ActorIDSet;
		std::vector<Ref<Actor>>                        Actors;

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