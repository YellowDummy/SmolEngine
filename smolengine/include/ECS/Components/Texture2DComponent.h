#pragma once
#include "ECS/Components/BaseComponent.h"

#include <string>
#include <glm/glm.hpp>
#include <cereal/cereal.hpp>
#ifndef FROSTIUM_SMOLENGINE_IMPL
#define FROSTIUM_SMOLENGINE_IMPL
#endif
#include <Frostium3D/Common/Texture.h>

namespace SmolEngine
{
	class Texture;

	struct Texture2DComponent: public BaseComponent
	{
		Texture2DComponent() = default;
		Texture2DComponent(uint32_t id)
			:BaseComponent(id) {}

		Ref<Texture>            Texture = nullptr;
		bool                    Enabled = true;
		int                     LayerIndex = 0;
		glm::vec4               Color = glm::vec4(1.0f);
		std::string             TexturePath = "";

	private:

		friend class cereal::access;
		friend class EditorLayer;
		friend class Scene;

		template<typename Archive>
		void serialize(Archive & archive)
		{
			archive(Color.r, Color.g, Color.b, Color.a, LayerIndex, TexturePath,  Enabled, ComponentID);
		}
	};
}