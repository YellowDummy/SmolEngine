#include "stdafx.h"
#include "Texture.h"
#include "Core/Core.h"
#include "Core/SLog.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/OpenGL/OpenglTexture.h"

#include <memory>

namespace SmolEngine
{
	void Texture2D::Bind(uint32_t slot) const
	{

#ifdef  SMOLENGINE_OPENGL_IMPL

		m_OpenglTexture2D.Bind(slot);
#else

#endif

	}

	void Texture2D::UnBind() const
	{

#ifdef  SMOLENGINE_OPENGL_IMPL

		m_OpenglTexture2D.UnBind();
#else

#endif

	}

	uint32_t Texture2D::GetHeight() const
	{

#ifdef  SMOLENGINE_OPENGL_IMPL

		return m_OpenglTexture2D.GetHeight();
#else

#endif

	}

	uint32_t Texture2D::GetWidth() const
	{

#ifdef  SMOLENGINE_OPENGL_IMPL

		return m_OpenglTexture2D.GetWidth();
#else

#endif

	}

	const uint32_t Texture2D::GetID() const
	{

#ifdef  SMOLENGINE_OPENGL_IMPL

		return m_OpenglTexture2D.GetID();
#else

#endif

	}

	void Texture2D::SetData(void* data, uint32_t size)
	{

#ifdef  SMOLENGINE_OPENGL_IMPL

		m_OpenglTexture2D.SetData(data, size);
#else

#endif

	}

	bool Texture2D::operator==(const Texture2D& other) const
	{
		return GetID() == other.GetID();
	}

	Ref<Texture2D> Texture2D::Create(const uint32_t width, const uint32_t height)
	{
		Ref<Texture2D> texture = std::make_shared<Texture2D>();

#ifdef  SMOLENGINE_OPENGL_IMPL

		texture->m_OpenglTexture2D.Init(width, height);
#else

#endif
		return texture;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& filePath)
	{
		Ref<Texture2D> texture = std::make_shared<Texture2D>();

#ifdef  SMOLENGINE_OPENGL_IMPL

		texture->m_OpenglTexture2D.Init(filePath);
#else

#endif
		return texture;
	}

	Ref<Texture2D> Texture2D::Create(FT_Bitmap* bitmap)
	{
		Ref<Texture2D> texture = std::make_shared<Texture2D>();

#ifdef  SMOLENGINE_OPENGL_IMPL

		texture->m_OpenglTexture2D.Init(bitmap);
#else

#endif
		return texture;
	}
}