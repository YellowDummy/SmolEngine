#pragma once
#include "Core/Core.h"
#include "Renderer/Texture.h"

namespace SmolEngine
{
	//Sprite sheet texture
	class SubTexture2D
	{
	public:

		SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);

		/// Getters
		
		const Ref<Texture2D> GetTexture() const { return m_Texture; }

		const glm::vec2* GetTextureCoods() const { return m_TextureCoods; }

		///
		
		static Ref<SubTexture2D> GenerateFromCoods(const Ref<Texture2D> texture, const glm::vec2& coods, const glm::vec2& cellSize,  const glm::vec2& spriteSize = { 1, 1 });

	private:

		friend class Scene;

		Ref<Texture2D> m_Texture;

		glm::vec2 m_TextureCoods[4];
	};
}
