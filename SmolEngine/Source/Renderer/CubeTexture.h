#pragma once
#include "Core/Core.h"

#ifdef SMOLENGINE_OPENGL_IMPL
#include "Renderer/OpenGL/OpenglTexture.h"
#else
#include "Renderer/Vulkan/VulkanTexture.h"
#endif // SMOLENGINE_OPENGL_IMPL


namespace SmolEngine
{
	class CubeTexture
	{
	public:

		static Ref<CubeTexture> Create(const std::string& filePath);

#ifndef SMOLENGINE_OPENGL_IMPL

		VulkanTexture* GetVulkanTexture();
#endif

	private:

#ifdef SMOLENGINE_OPENGL_IMPL
#else
		VulkanTexture m_VulkanTetxure = {};
#endif
	};
}