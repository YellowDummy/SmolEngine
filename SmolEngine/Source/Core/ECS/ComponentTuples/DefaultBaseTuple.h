#pragma once
#include "Core/Core.h"
#include "Core/ECS/ComponentTuples/BaseTuple.h"

#include "Core/ECS/Components/HeadComponent.h"
#include "Core/ECS/Components/TransformComponent.h"

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

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Info);
		}
	};
}