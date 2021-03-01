#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"

#include <glm/glm.hpp>
#include <cereal/cereal.hpp>

namespace SmolEngine
{
	struct PointLightComponent: public BaseComponent
	{
		PointLightComponent() = default;

		PointLightComponent(uint32_t id)
			:BaseComponent(id) {}

		// Data

		bool          bEnabled = true;

		float         Constant = 0.5f;
		float         Linear = 0.1f;
		float         Exposure = 0.0080f;

		glm::vec3     Offset = glm::vec3(0.0f);
		glm::vec4     Color = glm::vec4(1.0f);
				      
	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(bEnabled, Constant, Linear, Exposure, Offset.x, Offset.y, Offset.z,
				Color.r, Color.g, Color.b, Color.a, ComponentID);
		}
	};
}