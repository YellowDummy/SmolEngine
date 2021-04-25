#pragma once

#include <glm/glm.hpp>

namespace Frostium
{
	struct MaterialProperties
	{
		// x - albedro, y = normal, z = metallic, w = roughness, state_2 x = AO

		glm::ivec4   States_1;
		glm::ivec4   States_2;

		glm::ivec4   Indexes_1;
		glm::ivec4   Indexes_2;

		glm::vec4    PBRValues;
	};

	class Material
	{
	public:

		MaterialProperties  m_MaterialProperties;
	};
}