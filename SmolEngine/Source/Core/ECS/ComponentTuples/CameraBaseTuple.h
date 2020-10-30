#pragma once
#include "Core/Core.h"

#include "Core/ECS/ComponentTuples/BaseTuple.h"

#include "Core/ECS/Components/HeadComponent.h"
#include "Core/ECS/Components/CameraComponent.h"
#include "Core/ECS/Components/TransformComponent.h"
#include "Core/ECS/Components/CanvasComponent.h"

#include <cereal/cereal.hpp>

namespace SmolEngine
{
	struct CameraBaseTuple: public BaseTuple
	{
		CameraBaseTuple();

		/// Accessors

		const HeadComponent& GetInfo() const { return Info; }

		TransformComponent& GetTransform() { return Transform; }

		CameraComponent& GetCamera() { return Camera; }

		CanvasComponent& GetCanvas() { return Canvas; }

	private:

		/// Components

		TransformComponent Transform;

		CameraComponent Camera;

		HeadComponent Info;

		CanvasComponent Canvas;

	private:

		friend class RendererSystem;

		friend class cereal::access;

		friend class CameraSystem;

		friend class EditorLayer;

		friend class WorldAdmin;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Info, Camera, Transform, Canvas);
		}
	};
}