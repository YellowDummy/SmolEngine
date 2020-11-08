#pragma once
#include "Core/Renderer/Texture.h"

namespace SmolEngine
{
	class OpenglTexture2D: public Texture2D
	{
	public:

		OpenglTexture2D(const std::string& filePath);

		OpenglTexture2D(const uint32_t width, const uint32_t height);

		OpenglTexture2D(FT_Bitmap* bitmap);

		~OpenglTexture2D();

		/// Binding

		void Bind(uint32_t slot = 0) const override;

		void UnBind() const override;

		/// Getters

		virtual const uint32_t GetID() const override { return m_RendererID; }

		inline uint32_t GetHeight() const override { return m_Height; }

		inline uint32_t GetWidth() const override { return m_Width; }


		/// Setters

		virtual void SetData(void* data, uint32_t size) override;

		///

		bool operator==(const Texture& other) const override { return m_RendererID == other.GetID(); }

	private:

		uint32_t m_Width = 0;

		uint32_t m_Height = 0;

		uint32_t m_Channels = 0;

		uint32_t m_RendererID = 0;

		uint32_t m_DataFromat = 0, m_InternalFormat = 0;

		///

		std::string m_FilePath = "";
	};
}
