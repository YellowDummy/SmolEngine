#pragma once
#include "Core/Core.h"

namespace SmolEngine
{
	struct MaterialProperties
	{
		int          UseAlbedroMap = false;
		int          UseNormalMap = false;
		int          UseMetallicMap = false;
		int          UseRoughnessMap = false;
		int          UseAOMap = false;

		int          AlbedroMapIndex = 0;
		int          NormalMapIndex = 0;
		int          MetallicMapIndex = 0;
		int          RoughnessMapIndex = 0;
		int          AOMapIndex = 0;

		float        Metallic = 0;
		float        Roughness = 0;
		float        Albedo = 1.0f;
		float        Specular = 9.0f;
	};

	class Material
	{
	public:

		MaterialProperties  m_MaterialProperties;
	};
}