#pragma once

#include "Core/Renderer/OpenGL/OpenglTexture.h"
#include "Core/Renderer/Vulkan/VulkanTexture.h"

#include <glm/glm.hpp>
#include <string>

namespace SmolEngine
{
	class Texture2D
	{
	public:

		Texture2D() = default;

		~Texture2D() = default;

		/// 
		/// Binding
		/// 

		void Bind(uint32_t slot = 0) const;

		void UnBind() const;

		/// 
		/// Getters
		/// 

		uint32_t GetHeight() const;

		uint32_t GetWidth() const;

		const uint32_t GetID() const;

#ifndef  SMOLENGINE_OPENGL_IMPL

		VulkanTexture* GetVulkanTexture() { return &m_VulkanTexture; }
#endif
		/// 
		/// Setters
		/// 

		void SetData(void* data, uint32_t size);

		/// 
		/// Factory
		/// 

		static Ref<Texture2D> Create(const uint32_t width, const uint32_t height);

		static Ref<Texture2D> Create(const std::string& filePath);

		static Ref<Texture2D> Create(FT_Bitmap* bitmap);

		/// 
		/// Operators
		/// 

		bool operator==(const Texture2D& other) const;

	private:

#ifdef  SMOLENGINE_OPENGL_IMPL

		OpenglTexture2D m_OpenglTexture2D = {};
#else
		VulkanTexture m_VulkanTexture = {};

#endif //  SMOLENGINE_OPENGL_IMPL

	};
}
