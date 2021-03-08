#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"
#include "Animation/SkeletalAnimationClip.h"

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

namespace SmolEngine
{
	struct AnimatorComponent: public BaseComponent
	{
		AnimatorComponent() = default;

		AnimatorComponent(uint32_t id)
			:BaseComponent(id) {}

		// Data

		std::vector<SkeletalAnimationClip> Clips;

	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(Clips, ComponentID);
		}
	};
}