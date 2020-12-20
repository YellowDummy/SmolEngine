#include "stdafx.h"
#include "TexturesPool.h"

namespace SmolEngine
{
	Ref<Texture2D> TexturesPool::GetTexture2D(size_t id)
	{
		return m_Textures[id];
	}

	Ref<Texture2D> TexturesPool::AddTexture2D(const std::string& filePath)
	{
		std::hash<std::string> hasher;
		size_t id = hasher(filePath);
		auto& result = m_Textures.find(id);
		if (result == m_Textures.end())
		{
			Ref<Texture2D> texture = std::make_shared<Texture2D>();
			m_Textures[id] = texture;
			return texture;
		}

		return result->second;
	}

	Ref<Texture2D> TexturesPool::AddTexture2D(uint32_t width, uint32_t height)
	{
		size_t id = width + height;
		if (m_Textures.find(id) == m_Textures.end())
		{
			Ref<Texture2D> texture = std::make_shared<Texture2D>();
			m_Textures[id] = texture;
			return texture;
		}

		return m_Textures[id];
	}

	bool TexturesPool::IsTexture2DLoaded(const std::string& filePath)
	{
		std::hash<std::string> hasher;
		size_t id = hasher(filePath);
		return m_Textures.find(id) != m_Textures.end();
	}
}