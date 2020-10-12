#include "stdafx.h"
#include "stb_image.h"
#include "OpenglTexture.h"
#include "glad/glad.h"
#include "Core/SLog.h"

namespace SmolEngine
{
	OpenglTexture2D::OpenglTexture2D(const std::string& filePath)
		:m_FilePath(filePath)
	{
		int height, width, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
		}

		if (data == nullptr)
		{
			NATIVE_ERROR("Texture not found!");
			abort();
		}

		GLenum openglFormat = 0, dataFormat = 0;

		if (channels == 4)
		{
			openglFormat = GL_RGBA8;
			dataFormat = GL_RGBA;

		}

		if (channels == 3)
		{
			openglFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		m_InternalFormat = openglFormat; m_DataFromat = dataFormat;

		if (openglFormat == 0 || dataFormat == 0)
		{
			NATIVE_ERROR("Invalid input parameters, channels: {}", channels);
			abort();
		}

		m_Width = width; m_Height = height; m_Channels = channels;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenglTexture2D::OpenglTexture2D(const uint32_t width, const uint32_t height)
		:m_Width(width), m_Height(height), m_Channels(0)
	{
		m_InternalFormat = GL_RGBA8, m_DataFromat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	}

	OpenglTexture2D::OpenglTexture2D(FT_Bitmap* bitmap)
	{
		m_Width = bitmap->width;
		m_Height = bitmap->rows;

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_Width, m_Height, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap->buffer);

	}

	OpenglTexture2D::~OpenglTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenglTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t b = m_DataFromat == GL_RGBA ? 4 : 3;

		if (size != m_Width * m_Height * b)
		{
			NATIVE_ERROR("Data must be a texture!");
			abort();
		}

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFromat, GL_UNSIGNED_BYTE, data);
	}

	void OpenglTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	void OpenglTexture2D::UnBind() const
	{

	}
}