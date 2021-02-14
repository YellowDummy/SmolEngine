#include "stdafx.h"
#include "SSAOGenerator.h"
#include "Renderer/Texture.h"

#include <random>

namespace SmolEngine
{
	void SSAOGenerator::Generate(Ref<Texture>& out_NoiseTexture, std::array<glm::vec4, 32>& out_Kernel)
	{
		// Sample kernel
		const uint32_t kernelCount = 32;
		std::default_random_engine rndEngine((unsigned)time(nullptr));
		std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

		for (uint32_t i = 0; i < kernelCount; ++i)
		{
			glm::vec3 sample(rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine));
			sample = glm::normalize(sample);
			sample *= rndDist(rndEngine);
			float scale = float(i) / float(kernelCount);
			scale = Lerp(0.1f, 1.0f, scale * scale);
			out_Kernel[i] = glm::vec4(sample * scale, 0.0f);
		}

		// Random noise
		std::array<glm::vec4, 16> noise;
		for (uint32_t i = 0; i < static_cast<uint32_t>(noise.size()); i++)
		{
			noise[i] = glm::vec4(rndDist(rndEngine) * 2.0f - 1.0f, rndDist(rndEngine) * 2.0f - 1.0f, 0.0f, 0.0f);
		}

		// Create Noice Texture
		out_NoiseTexture = Texture::Create(noise.data(), noise.size() * sizeof(glm::vec4), 4, 4, TextureFormat::R32G32B32A32_SFLOAT);
	}

	float SSAOGenerator::Lerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}
}
