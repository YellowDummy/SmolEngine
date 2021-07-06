#pragma once
#include "Common/Core.h"
#include "Common/BufferLayout.h"

#include <glm/glm.hpp>

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	struct GraphicsContextState
	{
		GraphicsContextState(bool cam, bool imgui, bool swapchain, bool autoResize)
			:UseEditorCamera(cam), UseImGUI(imgui), UseSwapchain(swapchain), AutoResize(autoResize) {}

		bool           WindowMinimized = false;
		bool           Is2DStoragePreAlloc = false;
		bool           IsStoragePreAlloc = false;

		const bool     AutoResize = true;
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
		glm::vec3 min = glm::vec3(0);
		glm::vec3 max = glm::vec3(0);

		void SetBoundingBox(const glm::vec3& min, const glm::vec3& max);
		void CalculateAABB(const glm::mat4& m);
	};

	enum class ImageFilter: int
	{
		NEAREST,
		LINEAR,
	};

	enum class ShaderType : int
	{
		Vertex    = 1,
		Fragment  = 2,
		Compute   = 4,
		Geometry  = 8,

		RayGen    = 10,
		RayMiss   = 20,
		RayHit    = 40,
	};

	inline ShaderType operator~ (ShaderType a) { return (ShaderType)~(int)a; }
	inline ShaderType operator| (ShaderType a, ShaderType b) { return (ShaderType)((int)a | (int)b); }
	inline ShaderType operator& (ShaderType a, ShaderType b) { return (ShaderType)((int)a & (int)b); }
	inline ShaderType operator^ (ShaderType a, ShaderType b) { return (ShaderType)((int)a ^ (int)b); }
	inline ShaderType& operator|= (ShaderType& a, ShaderType b) { return (ShaderType&)((int&)a |= (int)b); }
	inline ShaderType& operator&= (ShaderType& a, ShaderType b) { return (ShaderType&)((int&)a &= (int)b); }
	inline ShaderType& operator^= (ShaderType& a, ShaderType b) { return (ShaderType&)((int&)a ^= (int)b); }
}