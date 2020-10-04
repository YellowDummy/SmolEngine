#pragma once
#include "Core/Core.h"

#include <map>
#include <string>
#include <cereal/cereal.hpp>
#include <cereal/types/map.hpp>

namespace SmolEngine
{
	class AudioEngine;
	struct AudioClip;

	class AudioSource
	{
	public:

		AudioSource();

		void OnAwake();

		void PlayClip(const std::string& keyName, float volume, bool loop);

		void StopClip(const std::string& keyName);

		void AddClip(Ref<AudioClip> clip);

	private:

		void ReloadAllClips();

		void DebugPlay(Ref<AudioClip> clip);

		void DebugStop(Ref<AudioClip> clip);

	private:

		std::map <std::string, Ref<AudioClip>> m_AudioClips;
		AudioEngine* m_AudioEngine = nullptr;

		bool m_PlayOnAwake = true;

	private:

		friend class cereal::access;
		friend class EditorLayer;
		friend class Scene;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(m_AudioClips, m_PlayOnAwake);
		}
	};
}