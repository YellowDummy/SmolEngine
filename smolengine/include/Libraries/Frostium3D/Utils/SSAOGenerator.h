#pragma once
#include "Common/Core.h"

#include <glm/glm.hpp>

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	class Texture;

	class SSAOGenerator
	{
	public:

		static void Generate(Ref<Texture>& out_NoiseTexture, std::array<glm::vec4, 32>& out_Kernel);

	private:

		static float Lerp(float a, float b, float f);
	};
}