#pragma once

#include "Core/Core.h"
#include "Renderer/Texture.h"
#include "Renderer/SubTexture.h"

#include <glm/glm.hpp>

namespace SmolEngine
{
	enum class DebugPrimitives : uint16_t
	{
		None = 0,
		Quad,
		Circle
	};

	struct DirectionalLightBuffer
	{
		glm::vec4 Position;
		glm::vec4 Color;
	};

	struct QuadVertex
	{
		glm::vec3 Position = glm::vec3(1.0f);
		glm::vec4 Color = glm::vec4(1.0f);
		glm::vec2 TextureCood = glm::vec2(1.0f);

		float TextMode = 0;
		float TextureID = 0.0f;
	};

	struct DebugVertex
	{
		glm::vec3 Position = glm::vec3(0.0f);
	};

	struct Light2DBuffer
	{
		Light2DBuffer() = default;

		Light2DBuffer(const glm::vec4 color, const glm::vec4 pos, float r, float intensity)
			:
			Color(color), Offset(pos), Attributes(r, intensity, 0, 0) {}


		glm::vec4 Color = glm::vec4(1.0f);
		glm::vec4 Offset = glm::vec4(1.0f);
		glm::vec4 Attributes = glm::vec4(1.0); // r = radius, g = intensity, b = 0, a = 0
	};

	struct LayerDataBuffer
	{
		LayerDataBuffer() = default;

		~LayerDataBuffer() { delete[] Base; }

		size_t ClearSize = 0;
		QuadVertex* Base = nullptr;
		QuadVertex* BasePtr = nullptr;

		uint32_t TextureSlotIndex = 1; // index 0 reserved for white texture
		std::vector<Ref<Texture>> TextureSlots;

		uint32_t IndexCount = 0;
		uint32_t QuadCount = 0;
		uint32_t LayerIndex = 0;
		bool isActive = false;
	};

	struct Renderer2DStats
	{
		uint32_t DrawCalls = 0;
		uint32_t QuadCount = 0;
		uint32_t TexturesInUse = 0;
		uint32_t LayersInUse = 0;

		/// Helpers

		void Reset()
		{
			DrawCalls = 0;
			QuadCount = 0;
			TexturesInUse = 0;
			LayersInUse = 0;
		}

		///  Getters

		inline uint32_t GetTotalVertexCount() { return QuadCount * 4; }

		inline uint32_t GetTotalIndexCount() { return QuadCount * 6; }
	};
}