#pragma once
#include "Core/Core.h"
#include "Core/ECS/ComponentTuples/BaseTuple.h"

#include "Core/ECS/Components/HeadComponent.h"
#include "Core/ECS/Components/Body2DComponent.h"
#include "Core/ECS/Components/TransformComponent.h"
#include "Core/ECS/Components/Texture2DComponent.h"

#include <cereal/cereal.hpp>

namespace SmolEngine
{
	struct PhysicsBaseTuple: public BaseTuple
	{
		PhysicsBaseTuple();

		/// Components

		TransformComponent Transform;

		HeadComponent Info;

		Texture2DComponent Texture;

		Body2DComponent Body;

	private:

		friend class EditorLayer;

		friend class Scene;

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Info, Body, Transform, Texture);
		}
	};
}