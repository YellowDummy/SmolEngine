#pragma once
#include "Core/Core.h"

#include <cereal/cereal.hpp>
#include <string>

namespace SmolEngine
{
	struct BehaviourComponent
	{
		BehaviourComponent();

		///

	private:

		friend class EditorLayer;

		friend class Scene;

		friend class cereal::access;
	};
}