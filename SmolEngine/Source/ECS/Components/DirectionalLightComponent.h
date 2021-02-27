#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"

#include <glm/glm.hpp>
#include <cereal/cereal.hpp>

namespace SmolEngine
{
	struct DirectionalLightComponent: public BaseComponent
	{
		DirectionalLightComponent() = default;

		DirectionalLightComponent(uint32_t id)
			:BaseComponent(id) {}

		bool      bCastShadows = false;
		bool      bEnabled = true;

		float     Intensity = 1.0f;

		glm::vec3 Direction = glm::vec3(0, 0, 0);
		glm::vec4 Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	private:

		friend class cereal::access;
		friend class EditorLayer;
		friend class WorldAdmin;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(bCastShadows, bEnabled, Intensity, Direction.x, Direction.y, Direction.z,
				Color.r, Color.g, Color.b, Color.a, ComponentID);
		}
	};
}