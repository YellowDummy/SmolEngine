#pragma once
#include "Common/Core.h"
#include "Common/Common.h"

#ifdef FROSTIUM_OPENGL_IMPL
#include "OpenGL/OpenglTexture.h"
#else
#include "Vulkan/VulkanTexture.h"
#endif // FROSTIUM_OPENGL_IMPL


namespace Frostium
{
	class CubeTexture
	{
	public:

		static Ref<CubeTexture> Create(const std::string& filePath, TextureFormat format = TextureFormat::R8G8B8A8_UNORM);

#ifndef FROSTIUM_OPENGL_IMPL
		VulkanTexture* GetVulkanTexture()
		{
			return &m_VulkanTetxure;
		}
#endif

	private:

#ifdef FROSTIUM_OPENGL_IMPL
#else
		VulkanTexture m_VulkanTetxure = {};
#endif
	};
}