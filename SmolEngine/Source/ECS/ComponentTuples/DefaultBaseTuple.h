#pragma once
#include "Core/Core.h"
#include "ECS/ComponentTuples/BaseTuple.h"

#include "ECS/Components/HeadComponent.h"
#include "ECS/Components/TransformComponent.h"

#include <cereal/cereal.hpp>

namespace SmolEngine
{
	struct DefaultBaseTuple: public BaseTuple
	{
		DefaultBaseTuple();

		/// Accessors

		const HeadComponent& GetInfo() const;

		/// Components

	private:

		HeadComponent Info;

	private:

		friend class cereal::access;
		friend class EditorLayer;
		friend class WorldAdmin;
		friend class Scene;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Info);
		}
	};
}