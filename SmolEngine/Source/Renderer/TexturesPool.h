#pragma once
#include "Core/Core.h"
#include "Renderer/Texture.h"

#include <unordered_map>

namespace SmolEngine
{
	class TexturesPool
	{
	public:

		static Ref<Texture> AddDummyTexture();

		static Ref<Texture> GetTexture(size_t id);

		static Ref<Texture> AddTexture(const std::string& filePath);

		static Ref<Texture> AddTexture(uint32_t width, uint32_t height);

		static void Reset();

		static bool IsTexture2DLoaded(const std::string& filePath);

		// Getters

		static std::unordered_map<size_t, Ref<Texture>>& GetTextures();

	private:

		inline static Ref<Texture> m_DummyTexture = nullptr;

		inline static std::unordered_map<size_t, Ref<Texture>> m_Textures;
	};
}