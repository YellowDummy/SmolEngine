#pragma once
#include "Common/Core.h"
#include "Common/Common.h"
#include "Primitives/Texture.h"

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	class CubeMap
	{
	public:
		Texture* GetTexture() const;

		static void Create(CubeMap* out_tex, const std::string& filePath, TextureFormat format = TextureFormat::R8G8B8A8_UNORM);
		static void CreateEmpty(CubeMap* out_tex, uint32_t width, uint32_t height, TextureFormat format = TextureFormat::R8G8B8A8_UNORM);
	private:
		Ref<Texture> m_Texture = nullptr;
	};
}