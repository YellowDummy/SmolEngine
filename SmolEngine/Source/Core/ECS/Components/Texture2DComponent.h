#pragma once

#include <string>
#include <glm/glm.hpp>
#include <cereal/cereal.hpp>

namespace SmolEngine
{
	class Texture2D;

	struct Texture2DComponent
	{
		Texture2DComponent();

		Texture2DComponent(const std::string& filePath, const glm::vec4& color = glm::vec4(1.0f));

		///

		glm::vec4 Color = glm::vec4(1.0f);

		///
		
		int LayerIndex = 0;

		///

		Ref<Texture2D> Texture = nullptr;

		///

		std::string TexturePath = "";

		std::string FileName = "";

		///

		bool Enabled = true;

	private:

		friend class EditorLayer;

		friend class Scene;

		friend class cereal::access;


		template<typename Archive>
		void serialize(Archive & archive)
		{
			archive(Color.r, Color.g, Color.b, Color.a, LayerIndex, TexturePath, FileName, Enabled);
		}
	};
}