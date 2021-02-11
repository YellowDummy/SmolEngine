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

	enum class ShaderType : uint32_t
	{
		Vertex,
		Fragment,
		Compute,
		Geometry
	};

	enum class TextureFormat
	{
		R8_UNORM,
		R8G8B8A8_UNORM,
		R32G32B32A32_SFLOAT,
		R16G16B16A16_SFLOAT // HDR
	};
}