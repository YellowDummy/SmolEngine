#pragma once

#include "Core/Core.h"

#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <entt/entity/registry.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>

namespace SmolEngine
{
	class Actor;

	struct SceneRegisrty
	{
		void Init();
		void Free();

		entt::registry m_Registry;
		entt::entity   m_Entity;
	};
}