#pragma once
#include "Core/Core.h"

#include <glm/glm.hpp>
#include <cereal/cereal.hpp>

namespace SmolEngine
{
	struct Light2DSourceComponent
	{
	public:

		Light2DSourceComponent() = default;

		/// Data

		glm::vec4 Color = glm::vec4(1.0f);

		glm::vec2 Position = glm::vec2(0.0f);

		//

		float Intensity = 1.0f;

		float Radius = 1.0f;

		//

		bool isEnabled = false;

	private:


		friend class EditorLayer;

		friend class Scene;

		friend class cereal::access;


		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Position.x, Position.y, Color.r, Color.g, Color.b, Color.a, Radius, Intensity, isEnabled);
		}
	};
}