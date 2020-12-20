#pragma once
#include "Core/Core.h"
#include "Core/Renderer/Texture.h"

#include <unordered_map>

namespace SmolEngine
{
	class TexturesPool
	{
	public:

		static Ref<Texture2D> GetTexture2D(size_t id);

		static Ref<Texture2D> AddTexture2D(const std::string& filePath);

		static Ref<Texture2D> AddTexture2D(uint32_t width, uint32_t height);


		static bool IsTexture2DLoaded(const std::string& filePath);

	private:

		inline static std::unordered_map<size_t, Ref<Texture2D>> m_Textures;
	};
}