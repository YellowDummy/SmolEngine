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

		/// Components

		TransformComponent Transform;

		CameraComponent Camera;

		HeadComponent Info;

		CanvasComponent Canvas;

	private:

		friend class EditorLayer;

		friend class Scene;

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Info, Camera, Transform, Canvas);
		}
	};
}