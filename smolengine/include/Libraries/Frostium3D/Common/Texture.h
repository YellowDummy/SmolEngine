#pragma once

#ifdef FROSTIUM_OPENGL_IMPL
#include "OpenGL/OpenglTexture.h"
#else
#include "Vulkan/VulkanTexture.h"
#endif

#include "Common/Common.h"

#include <glm/glm.hpp>
#include <string>

namespace Frostium
{
	struct TextureLoadedData;

	class Texture
	{
	public:

		Texture() = default;
		~Texture() = default;
		bool operator==(const Texture& other) const;

		void Bind(uint32_t slot = 0) const;
		void UnBind() const;
		bool IsInitialized() const;

		// Getters
		uint32_t GetHeight() const;
		uint32_t GetWidth() const;
		const uint32_t GetID() const;
		void* GetImGuiTexture() const;
#ifndef  FROSTIUM_OPENGL_IMPL
		VulkanTexture* GetVulkanTexture() { return &m_VulkanTexture; }
#endif
		// Factory
		static void CreateWhiteTexture(Texture* out_texture);
		static void Create(const std::string& filePath, Texture* out_texture, TextureFormat format = TextureFormat::R8G8B8A8_UNORM, bool flip = true, bool imgui_handler = false);
		static void Create(const void* data, uint32_t size, const uint32_t width, const uint32_t height, Texture* out_texture, TextureFormat format = TextureFormat::R8G8B8A8_UNORM);

	private:

		uint32_t        m_ID = 0;
#ifdef  FROSTIUM_OPENGL_IMPL
		OpenglTexture2D m_OpenglTexture2D = {};
#else
		VulkanTexture   m_VulkanTexture = {};
#endif
	};
}
