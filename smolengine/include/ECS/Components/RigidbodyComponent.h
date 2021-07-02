#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"
#include "Physics/Bullet3/RigidBody.h"

#include <cereal/cereal.hpp>

namespace SmolEngine
{
	class Actor;

	struct RigidbodyComponent : public BaseComponent, RigidBody
	{
		RigidbodyComponent() = default;
		RigidbodyComponent(uint32_t id)
			:BaseComponent(id), RigidBody() {}

		BodyCreateInfo  CreateInfo{};

	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(CreateInfo, ComponentID);
		}
	};
}