#include "stdafx.h"
#include "CubeTexture.h"

namespace SmolEngine
{
	Ref<CubeTexture> SmolEngine::CubeTexture::Create(const std::string& filePath)
	{
		Ref<CubeTexture> texture = std::make_shared<CubeTexture>();
#ifdef  SMOLENGINE_OPENGL_IMPL
#else
		texture->m_VulkanTetxure.CreateCubeMapKtx(filePath);
#endif
		return texture;
	}

	VulkanTexture* CubeTexture::GetVulkanTexture()
	{
		return &m_VulkanTetxure;
	}
}
