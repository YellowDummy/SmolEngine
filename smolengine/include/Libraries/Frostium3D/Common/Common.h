#pragma once
#include "Common/Core.h"
#include "Common/BufferLayout.h"

#include <glm/glm.hpp>

namespace Frostium
{
	struct GraphicsContextState
	{
		GraphicsContextState(bool cam, bool imgui, bool swapchain)
			:UseEditorCamera(cam), UseImGUI(imgui), UseSwapchain(swapchain) {}

		bool           WindowMinimized = false;
		bool           Is2DStoragePreAlloc = false;
		bool           IsStoragePreAlloc = false;
		const bool     UseImGUI = false;
		const bool     UseEditorCamera = false;
		const bool     UseSwapchain = true;
	};

	struct VertexInputInfo
	{
		VertexInputInfo() = default;
		VertexInputInfo(uint32_t stride, const BufferLayout& layout, bool isInputRateInstance = false)
			:
			Stride(stride),
			Layout(layout),
			IsInputRateInstance(isInputRateInstance) {}

		uint32_t          Stride = 0;
		BufferLayout      Layout{};
		bool              IsInputRateInstance = false;
	};

	struct PBRVertex
	{
		glm::vec3          Pos = glm::vec3(0.0f);
		glm::vec3          Normals = glm::vec3(0.0f);
		glm::vec3          Tangent = glm::vec3(0.0f);
		glm::vec2          UVs = glm::vec2(0.0f);
		glm::ivec4         jointIndices = glm::ivec4(0);
		glm::vec4          jointWeight = glm::vec4(0.0f);
	};

	struct TextVertex
	{
		glm::vec3 Pos;
		glm::vec2 UV;
	};

	struct BoundingBox
	{
		glm::vec3 min;
		glm::vec3 max;

		void CalculateAABB(const glm::mat4& m);
	};

	enum class ShaderType : uint32_t
	{
		Vertex,
		Fragment,
		Compute,
		Geometry,

		//Raytracing
		RayGen,
		RayMiss,
		RayHit
	};

	enum class TextureFormat
	{
		R8_UNORM,
		R8G8B8A8_UNORM,
		B8G8R8A8_UNORM,
		R32G32B32A32_SFLOAT,
		R16G16B16A16_SFLOAT // HDR
	};
}