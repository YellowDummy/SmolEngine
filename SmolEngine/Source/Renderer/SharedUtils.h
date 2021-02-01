#pragma once
#include "Core/Core.h"
#include "Renderer/BufferLayout.h"

#include <glm/glm.hpp>

namespace SmolEngine
{
	struct VertexInputInfo
	{
		VertexInputInfo(uint32_t stride, const BufferLayout& layout, bool isInputRateInstance = false)
			:
			Stride(stride),
			Layout(layout),
			IsInputRateInstance(isInputRateInstance) {}

		uint32_t          Stride;
		BufferLayout      Layout;
		bool              IsInputRateInstance;
	};

	struct PBRVertex
	{
		glm::vec3          Pos;
		glm::vec3          Normals;
		glm::vec4          Tangent;
		glm::vec2          UVs;
		glm::vec4          Color;
	};

	struct PBRVertexInstanced
	{
		glm::vec3           Position;
		glm::vec3           Rotation;
		glm::vec3           Scale;

		float               Albedo = 0.1f;
		float               Metallic = 0.1f;
		float               Roughness = 0.1f;

		int                 AlbedoSamplerIndex = 0;
		int                 MetallicSamplerIndex = 0;
		int                 RoughnessSamplerIndex = 0;
	};
}