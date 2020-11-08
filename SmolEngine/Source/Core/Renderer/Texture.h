#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Core/Core.h"

extern "C" {
#include <ft2build.h>
#include FT_FREETYPE_H
}

namespace SmolEngine
{
	class Texture
	{
	public:

		virtual ~Texture() = default;

		virtual bool operator==(const Texture& other) const = 0;

		/// Binding
		
		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual void UnBind() const = 0;

		/// Getters

		virtual uint32_t GetHeight() const = 0;

		virtual uint32_t GetWidth() const = 0;

		virtual const uint32_t GetID() const = 0;

		/// Setters

		virtual void SetData(void* data, uint32_t size) = 0;
	};

	class Texture2D : public Texture
	{
	public:

		Texture2D() = default;

		virtual ~Texture2D() = default;

		/// Factory

		static Ref<Texture2D> Create(const uint32_t width, const uint32_t height);

		static Ref<Texture2D> Create(const std::string& filePath);

		static Ref<Texture2D> Create(FT_Bitmap* bitmap);
	};
}
