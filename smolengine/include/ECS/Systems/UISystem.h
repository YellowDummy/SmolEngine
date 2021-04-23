#pragma once
#include "Core/Core.h"

#include <glm/glm.hpp>
#include <Frostium3D/Common/Events.h>

namespace SmolEngine
{
	struct CanvasComponent;

	class UISystem
	{
	public:

		UISystem() = default;

	private:

		friend class cereal::access;
		friend class WorldAdmin;
		friend class EditorLayer;
		friend class UILayer;
	};
}