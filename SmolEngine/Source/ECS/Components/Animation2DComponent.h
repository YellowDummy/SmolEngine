#pragma once
#include "Core/Core.h"
#include "ECS/Components/BaseComponent.h"
#include "Animation/AnimationClip2D.h"

#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>

namespace SmolEngine
{
	struct Animation2DComponent: public BaseComponent
	{
		Animation2DComponent();

		Animation2DComponent(uint32_t id)
			:BaseComponent(id) {}

		/// Data
		
		std::unordered_map<std::string, Ref<AnimationClip2D>> m_Clips;
		Ref<AnimationClip2D> CurrentClip = nullptr;
		int IndexLayer = 0;

	private:

		friend class cereal::access;
		friend class EditorLayer;
		friend class WorldAdmin;

		template<typename Archive>
		void serialize(Archive & archive)
		{
			archive(m_Clips, CurrentClip, IndexLayer, ComponentID);
		}
	};
}