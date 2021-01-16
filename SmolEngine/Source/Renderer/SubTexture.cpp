#include "stdafx.h"
#include "SubTexture.h"

namespace SmolEngine
{
	SubTexture2D::SubTexture2D(const Ref<Texture>& texture, const glm::vec2& min, const glm::vec2& max)
		:m_Texture(texture)
	{
		m_TextureCoods[0] = { min.x, min.y };
		m_TextureCoods[1] = { max.x, min.y };
		m_TextureCoods[2] = { max.x, max.y };
		m_TextureCoods[3] = { min.x, max.y };
	}

	Ref<SubTexture2D> SubTexture2D::GenerateFromCoods(const Ref<Texture> texture, const glm::vec2& coods, const glm::vec2& cellSize, const glm::vec2& spriteSize)
	{
		glm::vec2 min = { (coods.x * cellSize.x) / texture->GetWidth(), (coods.y * cellSize.y) / texture->GetHeight() };
		glm::vec2 max = { ((coods.x + spriteSize.x) * cellSize.x) / texture->GetWidth(), ((coods.y + spriteSize.y) * cellSize.y) / texture->GetHeight() };

		return std::make_shared<SubTexture2D>(texture, min, max);
	}
}