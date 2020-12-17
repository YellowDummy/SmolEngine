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
#endif
	}

	void Texture2D::UnBind() const
	{
#ifdef  SMOLENGINE_OPENGL_IMPL
		m_OpenglTexture2D.UnBind();
#endif
	}

	uint32_t Texture2D::GetHeight() const
	{
#ifdef  SMOLENGINE_OPENGL_IMPL

		return m_OpenglTexture2D.GetHeight();
#else

		return m_VulkanTexture.GetHeight();
#endif
	}

	uint32_t Texture2D::GetWidth() const
	{
#ifdef  SMOLENGINE_OPENGL_IMPL

		return m_OpenglTexture2D.GetWidth();
#else
		return m_VulkanTexture.GetWidth();
#endif
	}

	const uint32_t Texture2D::GetID() const
	{
#ifdef  SMOLENGINE_OPENGL_IMPL

		return m_OpenglTexture2D.GetID();
#else
		return 0;
#endif
	}

	void* Texture2D::GetImGuiTexture() const
	{
#ifdef  SMOLENGINE_OPENGL_IMPL

		return reinterpret_cast<void*>(m_OpenglTexture2D.GetID());
#else
		return m_VulkanTexture.GetImGuiTextureID();
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
		texture->Create(width, height);
#endif
		return texture;
	}

	Ref<Texture2D> Texture2D::CreateWhiteTexture()
	{
		Ref<Texture2D> texture = std::make_shared<Texture2D>();

#ifdef  SMOLENGINE_OPENGL_IMPL

		uint32_t whiteTextureData = 0xffffffff;
		texture->m_OpenglTexture2D.Init(1, 1);
		texture->m_OpenglTexture2D.SetData(&whiteTextureData, sizeof(uint32_t));
#else
		texture->m_VulkanTexture.CreateWhiteTetxure2D(1, 1);
#endif
		return texture;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& filePath)
	{
		Ref<Texture2D> texture = std::make_shared<Texture2D>();

#ifdef  SMOLENGINE_OPENGL_IMPL

		texture->m_OpenglTexture2D.Init(filePath);
#else
		texture->m_VulkanTexture.CreateTexture2D(filePath);
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