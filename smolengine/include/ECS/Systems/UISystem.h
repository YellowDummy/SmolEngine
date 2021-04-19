#pragma once
#include "Core/Core.h"

#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <Frostium3D/Common/Events.h>

namespace SmolEngine
{
	struct CanvasComponent;

	class UISystem
	{
	public:

		UISystem() = default;

		static void OnUpdate(entt::registry& registry);
		static void OnEvent(entt::registry& registry, Frostium::Event& e);

	private:

		friend class cereal::access;
		friend class WorldAdmin;
		friend class EditorLayer;
		friend class UILayer;
	};
}