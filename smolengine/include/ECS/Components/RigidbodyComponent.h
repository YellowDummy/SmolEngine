#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"
#include "Physics/PhysX/RigidBody.h"

#include <cereal/cereal.hpp>

namespace SmolEngine
{
	class Actor;

	struct RigidbodyComponent : public BaseComponent
	{
		RigidbodyComponent() = default;
		RigidbodyComponent(uint32_t id)
			:BaseComponent(id) {}

		BodyCreateInfo  CreateInfo{};
		RigidBody       DynamicBody{};

	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(CreateInfo, ComponentID);
		}
	};
}