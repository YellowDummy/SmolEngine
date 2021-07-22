#pragma once
#include "ECS/Components/BaseComponent.h"

#ifndef FROSTIUM_SMOLENGINE_IMPL
#define FROSTIUM_SMOLENGINE_IMPL
#endif
#include <Frostium3D/Common/RendererStorage.h>

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
		friend class EditorLayer;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Path, Intensity, IntensityBase);
		}
	};

	struct RendererStateComponent : public BaseComponent
	{
		RendererStateComponent() = default;
		RendererStateComponent(uint32_t id)
			:BaseComponent(id) {}

		enum class ShadowType : int
		{
			None,
			Hard,
			Soft
		};

		CubeMap*             CubeMap = nullptr;
		bool                 bGeneratePBRMaps = true;
		ShadowType           eShadowType = ShadowType::None;
		std::string          CubeMapPath = "";
		RendererState        State = {};
		DirectionalLight     DirLight = {};
		DynamicSkyProperties SkyProperties = {};
		Mask                 DirtMask = {};

	private:
		// internal use
		int                  PostProcessingFlag = 0;
		int                  DebugViewFlag = 0;
		int                  EnvironmentFlags = 0;
		int                  ShadowsFlags = 0;

	private:
		friend class cereal::access;
		friend class EditorLayer;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(eShadowType,

				State.bDrawGrid, State.bIBL, State.bDrawSkyBox, State.bFXAA, State.bBloom,
				State.Lighting.AmbientColor.r, State.Lighting.AmbientColor.g, State.Lighting.AmbientColor.b,
				State.Lighting.IBLStrength,
				State.Bloom.Exposure, State.Bloom.Scale, State.Bloom.Strength, State.Bloom.Threshold,
				State.FXAA.EdgeThresholdMax, State.FXAA.EdgeThresholdMin, State.FXAA.Iterations, State.FXAA.SubPixelQuality,

				DirLight.Color.r, DirLight.Color.g, DirLight.Color.b, DirLight.Color.a,
				DirLight.Direction.x, DirLight.Direction.y, DirLight.Direction.z, DirLight.Direction.w,
				DirLight.Intensity,
				DirLight.Bias,
				DirLight.zFar,
				DirLight.zNear,
				DirLight.lightFOV,
				DirLight.IsActive,
				DirLight.IsCastShadows,
				DirLight.IsUseSoftShadows,

				CubeMapPath, bGeneratePBRMaps, DirtMask,

				SkyProperties.AtmosphereRadius,
				SkyProperties.MieScale,
				SkyProperties.MieScatteringCoeff,
				SkyProperties.MieScatteringDirection,
				SkyProperties.PlanetRadius,
				SkyProperties.RayleighScale,
				SkyProperties.RayleighScatteringCoeff.x,
				SkyProperties.RayleighScatteringCoeff.y,
				SkyProperties.RayleighScatteringCoeff.z,
				SkyProperties.RayOrigin.x,
				SkyProperties.RayOrigin.y,
				SkyProperties.RayOrigin.z,
				SkyProperties.SunIntensity,
				SkyProperties.SunPosition.x,
				SkyProperties.SunPosition.y,
				SkyProperties.SunPosition.z,
				SkyProperties.NumCirrusCloudsIterations,
				SkyProperties.NumCumulusCloudsIterations);
		}
	};
}