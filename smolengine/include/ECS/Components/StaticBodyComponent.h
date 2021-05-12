#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"
#include "Physics/PhysX/StaticBody.h"

#include <cereal/cereal.hpp>

namespace SmolEngine
{
	class Actor;

	struct StaticbodyComponent : public BaseComponent
	{
		StaticbodyComponent() = default;
		StaticbodyComponent(uint32_t id)
			:BaseComponent(id) {}

		BodyCreateInfo  CreateInfo{};
		StaticBody      StaticBody{};

	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(CreateInfo, ComponentID);
		}
	};
}