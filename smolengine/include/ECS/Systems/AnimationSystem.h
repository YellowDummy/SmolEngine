#pragma once
#include "Core/Core.h"

#include <entt/entt.hpp>

namespace SmolEngine
{
	class AnimationSystem
	{
	public:

		static void OnSceneStart(entt::registry& registry);
		static void OnSceneEnd(entt::registry& registry);
		static void Update(entt::registry& registry);

	private:
	};
}