#pragma once
#include "Core/Core.h"
#include "Core/ECS/ComponentTuples/BaseTuple.h"

#include "Core/ECS/Components/AudioSourceComponent.h"
#include "Core/ECS/Components/Animation2DComponent.h"

#include <cereal/cereal.hpp>

namespace SmolEngine
{
	struct TransformComponent;

	struct HeadComponent;

	///

	struct ResourceTuple: public BaseTuple
	{
		ResourceTuple() = default;

		/// Components

		AudioSourceComponent AudioSource;

		Animation2DComponent Animation2D;

	private:

		friend class EditorLayer;

		friend class Scene;

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(AudioSource, Animation2D);
		}
	};
}