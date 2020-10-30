#pragma once
#include "Core/Core.h"
#include "Core/Audio/AudioClip.h"
#include "Core/Audio/AudioSource.h"

#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>

namespace SmolEngine
{

	struct AudioSourceComponent
	{
		AudioSourceComponent();

		///

		std::unordered_map <std::string, Ref<AudioClip>> AudioClips;

		///

		bool PlayOnAwake = true;

	private:

		friend class EditorLayer;

		friend class WorldAdmin;

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(AudioClips, PlayOnAwake);
		}
	};
}