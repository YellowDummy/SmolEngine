#pragma once
#include "Core/Core.h"
#include "Core/ECS/ComponentTuples/BaseTuple.h"

#include "Core/ECS/Components/HeadComponent.h"
#include "Core/ECS/Components/Texture2DComponent.h"
#include "Core/ECS/Components/TransformComponent.h"
#include "Core/ECS/Components/Light2DSourceComponent.h"

#include <cereal/cereal.hpp>

namespace SmolEngine
{
	struct DefaultBaseTuple: public BaseTuple
	{
		DefaultBaseTuple();

		/// Components

		TransformComponent Transform;

		Light2DSourceComponent Light2D;

		HeadComponent Info;

		Texture2DComponent Texture;

	private:

		friend class EditorLayer;

		friend class WorldAdmin;

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Info, Transform, Texture, Light2D);
		}
	};
}