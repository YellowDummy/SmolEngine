#pragma once

#include "Core/Core.h"
#include "Core/Renderer/Texture.h"
#include "Core/Renderer/SubTexture.h"

#include <glm/glm.hpp>

namespace SmolEngine
{
	// Vextex Data


	struct QuadVertex
	{
		glm::vec3 Position = glm::vec3(0.0f);

		glm::vec4 Color = glm::vec4(1.0f);

		glm::vec2 TextureCood = glm::vec2(1.0f);

		///

		float TextureID = 0.0f;
	};


	// Light Data


	struct Light2DBuffer
	{
		Light2DBuffer() = default;

		Light2DBuffer(const glm::vec4 color, const glm::vec3 pos, float r, float intensity)
			:
			Color(color), Offset(pos), Radius(r), Intensity(intensity) {}

		///

		glm::vec3 Color = glm::vec3(1.0f);

		glm::vec3 Offset = glm::vec3(1.0f);

		///

		float Radius = 1.0f;

		float Intensity = 1.0f;
	};


	// Layer Data


	struct LayerDataBuffer
	{
		~LayerDataBuffer()
		{
			delete[] Base;
		}

		//

		static const uint32_t MaxTextureSlot = 32; // TODO: get TMU (Texture Mapping Units) at initialization

		std::array<Ref<Texture2D>, MaxTextureSlot> TextureSlots;

		//

		QuadVertex* Base = nullptr;

		QuadVertex* BasePtr = nullptr;

		//

		uint32_t IndexCount = 0;

		uint32_t QuadCount = 0;

		uint32_t TextureSlotIndex = 1; // index 0 reserved for white texture

		//

		bool isActive = false;
	};


	// Debug Data


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

		/// Getters

		inline uint32_t GetTotalVertexCount() { return QuadCount * 4; }

		inline uint32_t GetTotalIndexCount() { return QuadCount * 6; }
	};


	enum class DebugPrimitives : uint16_t
	{
		None = 0,

		Quad,

		Circle
	};
}