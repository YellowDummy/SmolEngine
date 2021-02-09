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
		int          UseAlbedroMap = 0;
		int          UseNormalMap = 0;
		int          UseMetallicMap = 0;
		int          UseRoughnessMap = 0;
		int          UseAOMap = 0;
			         
		int          AlbedroMapIndex = 0;
		int          NormalMapIndex = 0;
		int          MetallicMapIndex = 0;
		int          RoughnessMapIndex = 0;
		int          AOMapIndex = 0;

		float        Metallic = 0;
		float        Roughness = 0;
	};
}