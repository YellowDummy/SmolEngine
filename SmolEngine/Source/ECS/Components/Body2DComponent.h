#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"
#include "Physics2D/Box2D/Body2D.h"

#include <cereal/cereal.hpp>


namespace SmolEngine
{
	class Actor;

	struct Body2DComponent: public BaseComponent
	{

		Body2DComponent();

		Body2DComponent(uint32 id);
		
		Body2DComponent(Ref<Actor> actor, int type);

		// Data

		Body2D           Body;
		uint32_t         ActorID = 0;
		bool             ShowShape = true;

	private:

		friend class cereal::access;
		friend class EditorLayer;
		friend class WorldAdmin;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Body, ActorID, ShowShape, ComponentID);
		}
	};
}