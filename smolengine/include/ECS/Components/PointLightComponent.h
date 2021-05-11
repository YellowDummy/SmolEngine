#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"

#include <Frostium3D/Common/RendererStorage.h>
#include <glm/glm.hpp>
#include <cereal/cereal.hpp>

namespace SmolEngine
{
	struct PointLightComponent: public BaseComponent
	{
		PointLightComponent() = default;
		PointLightComponent(uint32_t id)
			:BaseComponent(id) {}

		Frostium::PointLight Light{};
				      
	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Light.Color.r, Light.Color.g, Light.Color.b, Light.Color.a,
				Light.Position.x, Light.Position.y, Light.Position.z, Light.Position.w,
				Light.Bias,
				Light.Intensity, 
				Light.IsActive,
				Light.IsCastShadows,
				ComponentID);
		}
	};
}