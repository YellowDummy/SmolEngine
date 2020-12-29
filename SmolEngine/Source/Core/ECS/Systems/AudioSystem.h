#pragma once
#include "Core/Core.h"

#include <entt.hpp>

namespace SmolEngine
{
	struct AudioSourceComponent;

	struct AudioClip;

	class AudioEngine;

	///

	class AudioSystem
	{
	public:

		AudioSystem();

		///

		static void OnAwake(entt::registry& registry, AudioEngine* audioEngine);

		static void OnReset(entt::registry& registry, AudioEngine* audioEngine);

		///

		static void PlayClip(const std::string& keyName, AudioSourceComponent& audioSource, AudioEngine* audioEngine);

		static void StopClip(const std::string& keyName, AudioSourceComponent& audioSource, AudioEngine* audioEngine);

		///

		static bool AddClip(AudioSourceComponent& audioSource, Ref<AudioClip> clip);

		static bool DeleteClip(AudioSourceComponent& audioSource, Ref<AudioClip> clip);

		///
		
		static bool RemoveClip(AudioSourceComponent& audioSource, const std::string& keyName);

		static bool RenameClip(AudioSourceComponent& audioSource, const std::string& keyName, const std::string& newName);

		static void ReloadAllClips(AudioSourceComponent& audioSource, AudioEngine* audioEngine);

		///
		
	private:

		static void DebugPlay(Ref<AudioClip> clip, AudioEngine* audioEngine);

		static void DebugStop(Ref<AudioClip> clip, AudioEngine* audioEngine);

		///

	private:

		friend class EditorLayer;
		friend class WorldAdmin;

	};
}