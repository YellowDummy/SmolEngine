#include "stdafx.h"
#include "CubeTexture.h"

namespace SmolEngine
{
	Ref<CubeTexture> SmolEngine::CubeTexture::Create(const std::string& filePath, TextureFormat format)
	{
		Ref<CubeTexture> texture = std::make_shared<CubeTexture>();
#ifdef  SMOLENGINE_OPENGL_IMPL
#else
		texture->m_VulkanTetxure.LoadCubeMap(filePath, format);
#endif
		return texture;
	}
}
