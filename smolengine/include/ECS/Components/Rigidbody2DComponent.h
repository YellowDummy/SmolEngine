#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"
#include "Physics/Box2D/Body2D.h"

#include <cereal/cereal.hpp>

namespace SmolEngine
{
	class Actor;

	struct Rigidbody2DComponent: public BaseComponent
	{
		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(uint32 id)
			:BaseComponent(id) {}

		bool             ShowShape = true;
		Ref<Actor>       Actor = nullptr;
		Body2D           Body = {};

	private:

		friend class cereal::access;
		friend class EditorLayer;
		friend class WorldAdmin;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Body, Actor, ShowShape, ComponentID);
		}
	};
}