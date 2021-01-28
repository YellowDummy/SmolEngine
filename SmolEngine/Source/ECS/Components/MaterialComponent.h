#pragma once
#include "Core/Core.h"
#include "Renderer/Texture.h"

#include <cereal/cereal.hpp>

namespace SmolEngine
{
	struct MaterialTexture
	{
		Ref<Texture> Texture = nullptr;
		std::string FilePath = "";

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(filePath);
		}
	};

	struct MaterialComponent
	{
		MaterialTexture          AlbedoTexture;
		MaterialTexture          MetallicTetxure;
		MaterialTexture          RoughnessTetxure;
		MaterialTexture          NormalTexture;

		float                    Albedo = 0.1f;
		float                    Metallic = 0.1f;
		float                    Roughness = 0.1f;

	public:

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(filePath);
		}
	};
}