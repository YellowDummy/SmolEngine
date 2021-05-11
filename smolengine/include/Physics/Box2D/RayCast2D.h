#pragma once

#include "Core/Core.h"

#include <box2d/b2_world_callbacks.h>
#include <glm/glm.hpp>

namespace SmolEngine
{
	class Actor;

	struct RayCast2DHitInfo
	{
		Actor*     Actor = nullptr;
		bool       IsBodyHitted = false;
		float      Fraction = 0.0f;
		glm::vec2  HitPoint = glm::vec2(0.0f);
		glm::vec2  Normal = glm::vec2(0.0f);
	};

	class RayCast2D: public b2RayCastCallback
	{
	public:

		virtual float ReportFixture(b2Fixture* fixture, const b2Vec2& point,
			const b2Vec2& normal, float fraction) override;

		const RayCast2DHitInfo& GetHitInfo();

	private:

		RayCast2DHitInfo m_Info;
	};
}