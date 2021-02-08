#pragma once
#include "Core/Core.h"

#include <glm/glm.hpp>

namespace SmolEngine
{
	class Texture;

	class SSAOGenerator
	{
	public:

		static void Generate(Ref<Texture>& out_NoiseTexture, std::array<glm::vec4, 64>& out_Kernel);

	private:

		static float Lerp(float a, float b, float f);
	};
}