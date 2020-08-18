#pragma once
#include "Core/Renderer/Texture.h"

namespace SmolEngine
{
	class OpenglTexture2D: public Texture2D
	{
	public:
		OpenglTexture2D(const std::string& filePath);
		OpenglTexture2D(const uint32_t width, const uint32_t height);
		~OpenglTexture2D();
		inline uint32_t GetHeight() const override { return m_Height; }
		inline uint32_t GetWidth() const override { return m_Width; }
		virtual void SetData(void* data, uint32_t size) override;
		void Bind(uint32_t slot = 0) const override;
		void UnBind() const override;
		virtual const uint32_t GetID() const override { return m_RendererID; }
		bool operator==(const Texture& other) const override { return m_RendererID == other.GetID(); }

	private:
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_Channels;
		uint32_t m_RendererID;
		std::string m_FilePath;

		unsigned int m_DataFromat, m_InternalFormat;
	};
}
