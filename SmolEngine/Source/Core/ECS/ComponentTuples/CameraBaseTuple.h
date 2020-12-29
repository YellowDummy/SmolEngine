#pragma once
#include "Core/Core.h"

#include "Core/ECS/ComponentTuples/BaseTuple.h"

#include "Core/ECS/Components/HeadComponent.h"
#include "Core/ECS/Components/CameraComponent.h"
#include "Core/ECS/Components/TransformComponent.h"

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

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Info, Camera);
		}
	};
}