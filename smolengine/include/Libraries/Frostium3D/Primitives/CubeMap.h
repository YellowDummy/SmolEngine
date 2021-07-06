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
	class CubeMap: Texture
	{
	public:
		Texture* GetTexture();

		static void Create(const TextureCreateInfo* info, CubeMap* out_tex);
		static void CreateEmpty(CubeMap* out_tex, uint32_t width, uint32_t height, TextureFormat format = TextureFormat::R8G8B8A8_UNORM);
	};
}