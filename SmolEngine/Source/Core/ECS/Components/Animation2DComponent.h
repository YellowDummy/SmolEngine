#pragma once
#include "Core/Core.h"
#include "Core/ECS/Components/BaseComponent.h"

#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>

namespace SmolEngine
{
	struct AnimationClip;

	struct Animation2DComponent: public BaseComponent
	{
		Animation2DComponent();

		Animation2DComponent(uint32_t id)
			:BaseComponent(id) {}

		/// Data
		
		std::unordered_map<std::string, Ref<AnimationClip>> m_Clips;
		Ref<AnimationClip> CurrentClip = nullptr;
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