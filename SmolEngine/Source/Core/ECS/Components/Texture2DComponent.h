#pragma once
#include "Core/ECS/Components/BaseComponent.h"

#include <string>
#include <glm/glm.hpp>
#include <cereal/cereal.hpp>

namespace SmolEngine
{
	class Texture2D;

	struct Texture2DComponent: public BaseComponent
	{
		Texture2DComponent();

		Texture2DComponent(uint32_t id);

		Texture2DComponent(const std::string& filePath, const glm::vec4& color = glm::vec4(1.0f));

	public:

		glm::vec4 Color = glm::vec4(1.0f);

		std::string TexturePath = "";
		std::string FileName = "";

		Ref<Texture2D> Texture = nullptr;

		int LayerIndex = 0;
		bool Enabled = true;

	private:

		friend class cereal::access;
		friend class EditorLayer;
		friend class Scene;

		template<typename Archive>
		void serialize(Archive & archive)
		{
			archive(Color.r, Color.g, Color.b, Color.a, LayerIndex, TexturePath, FileName, Enabled, ComponentID);
		}
	};
}