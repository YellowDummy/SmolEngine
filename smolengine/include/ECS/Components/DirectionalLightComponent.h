#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"

#include <Frostium3D/Common/RendererStorage.h>
#include <glm/glm.hpp>
#include <cereal/cereal.hpp>

namespace SmolEngine
{
	struct DirectionalLightComponent: public BaseComponent
	{
		DirectionalLightComponent() = default;
		DirectionalLightComponent(uint32_t id)
			:BaseComponent(id) {}

		Frostium::DirectionalLight   Light{};

	private:

		friend class cereal::access;
		friend class EditorLayer;
		friend class WorldAdmin;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Light.Color.r, Light.Color.g, Light.Color.b, Light.Color.a, 
				Light.Direction.x, Light.Direction.y, Light.Direction.z, Light.Direction.w,
				Light.Intensity,
				Light.Bias,
				Light.IsActive,
				Light.IsCastShadows,
				Light.IsUseSoftShadows,
				ComponentID);
		}
	};
}