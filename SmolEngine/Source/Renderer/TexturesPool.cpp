#include "stdafx.h"
#include "TexturesPool.h"

namespace SmolEngine
{
	Ref<Texture> TexturesPool::AddDummyTexture()
	{
		if (m_DummyTexture != nullptr)
			return m_DummyTexture;

		m_DummyTexture = std::make_shared<Texture>();
		return m_DummyTexture;
	}

	Ref<Texture> TexturesPool::GetTexture(size_t id)
	{
		return m_Textures[id];
	}

	Ref<Texture> TexturesPool::AddTexture(const std::string& filePath)
	{
		std::hash<std::string> hasher;
		size_t id = hasher(filePath);
		auto& result = m_Textures.find(id);
		if (result == m_Textures.end())
		{
			Ref<Texture> texture = std::make_shared<Texture>();
			m_Textures[id] = texture;
			return texture;
		}

		return result->second;
	}

	Ref<Texture> TexturesPool::AddTexture(uint32_t width, uint32_t height)
	{
		size_t id = width + height;
		if (m_Textures.find(id) == m_Textures.end())
		{
			Ref<Texture> texture = std::make_shared<Texture>();
			m_Textures[id] = texture;
			return texture;
		}

		return m_Textures[id];
	}

	void TexturesPool::Reset()
	{
		m_Textures.clear();
	}

	bool TexturesPool::IsTexture2DLoaded(const std::string& filePath)
	{
		std::hash<std::string> hasher;
		size_t id = hasher(filePath);
		return m_Textures.find(id) != m_Textures.end();
	}

	std::unordered_map<size_t, Ref<Texture>>& TexturesPool::GetTextures()
	{
		return m_Textures;
	}
}