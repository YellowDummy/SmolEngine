#pragma once
#include "Core/Core.h"
#include "Core/Physics2D/Box2D/Body2D.h"

#include <cereal/cereal.hpp>


namespace SmolEngine
{
	class Actor;

	struct Body2DComponent
	{

		Body2DComponent();
		
		Body2DComponent(Ref<Actor> actor, int type);

		/// Data

		Body2D Body;

		size_t ActorID = 0;

		bool ShowShape = true;

	private:

		friend class EditorLayer;

		friend class Scene;

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Body, ActorID, ShowShape);
		}
	};
}