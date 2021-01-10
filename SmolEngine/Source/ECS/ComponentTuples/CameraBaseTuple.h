#pragma once
#include "Core/Core.h"

#include "ECS/ComponentTuples/BaseTuple.h"

#include "ECS/Components/HeadComponent.h"
#include "ECS/Components/CameraComponent.h"
#include "ECS/Components/TransformComponent.h"

#include <cereal/cereal.hpp>

namespace SmolEngine
{
	struct CameraBaseTuple: public BaseTuple
	{
		CameraBaseTuple();

		/// Accessors

		const HeadComponent& GetInfo() const { return Info; }

		CameraComponent& GetCamera() { return Camera; }

	private:

		/// Components

		CameraComponent Camera;
		HeadComponent Info;

	private:

		friend class cereal::access;
		friend class RendererSystem;
		friend class CameraSystem;
		friend class EditorLayer;
		friend class WorldAdmin;
		friend class Scene;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Info, Camera);
		}
	};
}