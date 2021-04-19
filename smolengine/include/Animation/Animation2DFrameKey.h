#pragma once
#include "Core/Core.h"

#include <Frostium3D/Common/Texture.h>
#include <glm/glm.hpp>
#include <string>
#include <cereal/cereal.hpp>

namespace SmolEngine
{
	struct Animation2DFrameKey
	{
		Animation2DFrameKey() = default;

		// Data

		float                       Speed = 100.0f;
		Ref<Frostium::Texture>      Texture = nullptr;
		glm::vec2                   TextureScale = glm::vec2(1.0f);
		glm::vec4                   TextureColor = glm::vec4(1.0f);
		std::string                 TexturePath = "";
		std::string                 FileName = "";

	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(TexturePath, FileName, Speed, TextureColor.r, TextureColor.g, 
				TextureColor.b, TextureColor.a, TextureScale.x, TextureScale.y);
		}
	};
}