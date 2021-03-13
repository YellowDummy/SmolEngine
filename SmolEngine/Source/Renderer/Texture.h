#pragma once

#include "Renderer/OpenGL/OpenglTexture.h"
#include "Renderer/Vulkan/VulkanTexture.h"
#include "Renderer/Shared.h"

#include <glm/glm.hpp>
#include <string>

namespace SmolEngine
{
	struct TextureLoadedData;

	class Texture
	{
	public:

		Texture() = default;

		~Texture() = default;

		/// Binding

		void Bind(uint32_t slot = 0) const;

		void UnBind() const;

		/// Getters

		uint32_t GetHeight() const;

		uint32_t GetWidth() const;

		const uint32_t GetID() const;

		void* GetImGuiTexture() const;

#ifndef  SMOLENGINE_OPENGL_IMPL

		VulkanTexture* GetVulkanTexture() { return &m_VulkanTexture; }
#endif
		/// Setters

		void SetData(void* data, uint32_t size);

		///

		static void LoadTexture(const std::string& path, TextureLoadedData* outData);

		/// Factory

		static Ref<Texture> Create(const std::string& filePath, TextureFormat format = TextureFormat::R8G8B8A8_UNORM, bool pooling = true);

		static Ref<Texture> Create(const TextureLoadedData* data, TextureFormat format = TextureFormat::R8G8B8A8_UNORM, bool pooling = true);

		static Ref<Texture> Create(const void* data, uint32_t size, const uint32_t width, const uint32_t height,
			TextureFormat format = TextureFormat::R8G8B8A8_UNORM);

		///

		static Ref<Texture> CreateWhiteTexture();

		/// Operators

		bool operator==(const Texture& other) const;

	private:

#ifdef  SMOLENGINE_OPENGL_IMPL
		OpenglTexture2D m_OpenglTexture2D = {};
#else
		VulkanTexture m_VulkanTexture = {};
#endif
		bool m_Initialized = false;
	};
}
