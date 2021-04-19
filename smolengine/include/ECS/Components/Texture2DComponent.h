#pragma once
#include "ECS/Components/BaseComponent.h"

#include <string>
#include <glm/glm.hpp>
#include <cereal/cereal.hpp>
#include <Frostium3D/Common/Texture.h>

namespace SmolEngine
{
	class Texture;

	struct Texture2DComponent: public BaseComponent
	{
		Texture2DComponent() = default;
		Texture2DComponent(uint32_t id)
			:BaseComponent(id) {}

		bool                    Enabled = true;
		Ref<Frostium::Texture>  Texture = nullptr;
		int                     LayerIndex = 0;
		glm::vec4               Color = glm::vec4(1.0f);
		std::string             TexturePath = "";
		std::string             FileName = "";

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