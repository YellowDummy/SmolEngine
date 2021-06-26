#pragma once

#include "Common/Framebuffer.h"
#include "Common/CubeMap.h"

#include "GraphicsPipeline.h"
#include <glm/glm.hpp>

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	struct DynamicSkyProperties
	{
		glm::vec4 RayOrigin = glm::vec4(0, 6372e3f, 0, 0);
		glm::vec4 SunPosition = glm::vec4(100.0f, 100.0f, 22, 0);
		glm::vec4 RayleighScatteringCoeff = glm::vec4(5.5e-6, 13.0e-6, 22.4e-6, 0);

		float SunIntensity = 22.0f;
		// Radius of the planet in meters
		float PlanetRadius = 6371e3f;
		// Radius of the atmosphere in meters
		float AtmosphereRadius = 6471e3;
		float MieScatteringCoeff = 21e-6f;

		// Rayleigh scale height
		float RayleighScale = 8e3f;
		// Mie scale height
		float MieScale = 1.2e3f;
		//Mie preferred scattering direction
		float MieScatteringDirection = 0.758f;

	private:

		float pad1 = 1.0f;
	};

	class EnvironmentMap
	{
	public:
		void                    Initialize();
		void                    GenerateStatic(CubeMap* cubeMap);
		void                    GenerateDynamic(const glm::mat4& cameraProj = glm::mat4(0));
		void                    UpdateDescriptors();
			                    
		bool                    IsReady() const;
		bool                    IsDynamic() const;
		CubeMap*                GetCubeMap() const;
		DynamicSkyProperties&   GetDynamicSkyProperties();
	private:
		void                    Free();
	private:
		bool                    m_IsDynamic = false;
		CubeMap*                m_CubeMap = nullptr;
		GraphicsPipeline        m_GraphicsPipeline = {};
		Framebuffer             m_Framebuffer = {};
		DynamicSkyProperties    m_UBO = {};
	};
}