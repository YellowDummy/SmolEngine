#pragma once
#include "Common/Core.h"
#include "Utils/GLM.h"

#include <array>

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	class Frustum
	{
	public:

		void Update(const glm::mat4& matrix);
		bool CheckSphere(const glm::vec3& pos, float radius = 1.0f) const;

	private:

		enum side
		{
			LEFT = 0,
			RIGHT = 1,
			TOP = 2,
			BOTTOM = 3,
			BACK = 4,
			FRONT = 5
		};

		std::array<glm::vec4, 6> planes;
	};
}