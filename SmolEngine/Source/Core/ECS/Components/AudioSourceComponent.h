#pragma once
#include "Core/Core.h"
#include "Core/ECS/Components/BaseComponent.h"
#include "Core/Audio/AudioClip.h"
#include "Core/Audio/AudioSource.h"

#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>

namespace SmolEngine
{
	struct AudioSourceComponent: public BaseComponent
	{
		AudioSourceComponent();

		AudioSourceComponent(uint32_t id)
			: BaseComponent(id) {}

		/// Data

		std::unordered_map <std::string, Ref<AudioClip>> AudioClips;
		bool PlayOnAwake = true;

	private:

		friend class cereal::access;
		friend class EditorLayer;
		friend class WorldAdmin;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(AudioClips, PlayOnAwake, ComponentID);
		}
	};
}