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

		/// Accessors

		const HeadComponent& GetInfo() const  { return Info; }
											  
		TransformComponent& GetTransform()    { return Transform; }
											  
		Texture2DComponent& GetTexture()      { return Texture; }
											  
		Body2DComponent& GetBody2D()          { return Body; }

	private:

		/// Components

		TransformComponent Transform;

		HeadComponent Info;

		Texture2DComponent Texture;

		Body2DComponent Body;

	private:

		friend class Box2DPhysicsSystem;

		friend class RendererSystem;

		friend class EditorLayer;

		friend class WorldAdmin;

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Info, Body, Transform, Texture);
		}
	};
}