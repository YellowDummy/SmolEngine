#pragma once
#include "Common/Core.h"
#include "Common/Texture.h"

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	class SubTexture2D
	{
	public:

		SubTexture2D(const Ref<Texture>& texture, const glm::vec2& min, const glm::vec2& max);
		static Ref<SubTexture2D> GenerateFromCoods(const Ref<Texture> texture, const glm::vec2& coods,
			const glm::vec2& cellSize,  const glm::vec2& spriteSize = { 1, 1 });

		// Getters
		const Ref<Texture> GetTexture() const { return m_Texture; }
		const glm::vec2* GetTextureCoods() const { return m_TextureCoods; }

	private:

		Ref<Texture>  m_Texture;
		glm::vec2     m_TextureCoods[4];

	private:

		friend class Scene;
	};
}
