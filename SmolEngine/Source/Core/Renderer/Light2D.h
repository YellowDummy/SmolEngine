#pragma once

#include <glm/glm.hpp>
#include <cereal/cereal.hpp>

namespace SmolEngine
{
	class Light2D
	{
	public:
		Light2D() = default;

		glm::vec4 m_Color = glm::vec4(1.0f);
		glm::vec2 m_Shape = glm::vec2(1.0f);

		float intensity = 1.0f;

	private:
		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(m_Color.r, m_Color.g, m_Color.b, m_Color.a, m_Shape.x, m_Shape.y, intensity);
		}
	};
}