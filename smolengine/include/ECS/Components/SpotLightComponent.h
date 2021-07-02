#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"

#ifndef FROSTIUM_SMOLENGINE_IMPL
#define FROSTIUM_SMOLENGINE_IMPL
#endif
#include <Frostium3D/Common/RendererStorage.h>

#include <glm/glm.hpp>
#include <cereal/cereal.hpp>

namespace SmolEngine
{
	struct SpotLightComponent : public BaseComponent, SpotLight
	{
		SpotLightComponent() = default;
		SpotLightComponent(uint32_t id)
			:BaseComponent(id), SpotLight() {}

	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Color.r, Color.g, Color.b, Color.a,
				Position.x, Position.y, Position.z,
				Direction.x, Direction.y, Direction.z,
				Intensity, CutOff, OuterCutOff, Raduis, Bias, IsActive, ComponentID);
		}
	};
}