#pragma once
#include "Core/Core.h"

#include "ECS/Components/BaseComponent.h"

#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/archives/json.hpp>

namespace SmolEngine
{
	struct CanvasComponent: public BaseComponent
	{
		CanvasComponent() = default;
		CanvasComponent(uint32_t id)
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