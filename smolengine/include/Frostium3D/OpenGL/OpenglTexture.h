#pragma once
#ifdef FROSTIUM_OPENGL_IMPL
#include "Common/Core.h"

#include <string>

namespace Frostium
{
	class OpenglTexture2D
	{
	public:

		OpenglTexture2D();
		~OpenglTexture2D();

		void Init(const std::string& filePath);
		void Init(const uint32_t width, const uint32_t height);

		// Binding
		void Bind(uint32_t slot = 0) const;
		void UnBind() const;

		// Getters
		const uint32_t GetID() const { return m_RendererID; }
		inline uint32_t GetHeight() const { return m_Height; }
		inline uint32_t GetWidth() const { return m_Width; }

		// Setters
		void SetData(void* data, uint32_t size);

	private:

		uint32_t      m_Width = 0;
		uint32_t      m_Height = 0;
		uint32_t      m_Channels = 0;
		uint32_t      m_RendererID = 0;
		uint32_t      m_DataFromat = 0;
		uint32_t	  m_InternalFormat = 0;

		std::string   m_FilePath = "";
	};
}

#endif