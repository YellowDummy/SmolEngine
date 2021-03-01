#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"

#include <glm/glm.hpp>
#include <cereal/cereal.hpp>

namespace SmolEngine
{
	struct Light2DSourceComponent: public BaseComponent
	{
	public:

		Light2DSourceComponent() = default;

		Light2DSourceComponent(uint32_t id)
			:BaseComponent(id) {}

		// Data

		bool         IsEnabled = false;
		float        Intensity = 1.0f;
		float        Radius = 1.0f;

		glm::vec2    Offset = glm::vec2(0.0f);
		glm::vec4    Color = glm::vec4(1.0f);

	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Offset.x, Offset.y, Color.r, Color.g, Color.b, Color.a, Radius, Intensity, IsEnabled, ComponentID);
		}
	};
}