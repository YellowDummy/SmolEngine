#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"

#include <cereal/cereal.hpp>

namespace SmolEngine
{
	struct Animation2DComponent: public BaseComponent
	{
		Animation2DComponent() = default;
		Animation2DComponent(uint32_t id)
			:BaseComponent(id) {}

	private:
		friend class cereal::access;
		friend class EditorLayer;
		friend class WorldAdmin;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(ComponentID);
		}

	};
}