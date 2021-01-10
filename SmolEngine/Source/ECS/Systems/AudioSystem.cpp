#include "stdafx.h"
#include "AudioSystem.h"

#include "ECS/ComponentsCore.h"
#include "Audio/AudioEngine.h"
#include "Core/SLog.h"

#include "Audio/AudioClip.h"

namespace SmolEngine
{
	AudioSystem::AudioSystem()
	{

	}

	void AudioSystem::OnAwake(entt::registry& registry, AudioEngine* audioEngine)
	{
		OnReset(registry, audioEngine);

		const auto& view = registry.view<AudioSourceComponent>();
		for (const auto& entity : view)
		{
			auto& audio = view.get<AudioSourceComponent>(entity);
			if (!audio.PlayOnAwake) { return; }

			for (const auto& pair : audio.AudioClips)
			{
				const auto& [key, clip] = pair;

				if (clip->isDefaultClip)
				{
					PlayClip(key, audio, audioEngine);
					return;
				}
			}
		}
	}

	void AudioSystem::OnReset(entt::registry& registry, AudioEngine* audioEngine)
	{
		const auto& view = registry.view<AudioSourceComponent>();
		for (const auto& entity : view)
		{
			auto& audio = view.get<AudioSourceComponent>(entity);
			for (const auto& pair : audio.AudioClips)
			{
				const auto& [key, clip] = pair;

				audioEngine->StopClip(clip->Channel);
			}
		}

	}

	void AudioSystem::PlayClip(const std::string& keyName, AudioSourceComponent& audioSource, AudioEngine* audioEngine)
	{
		auto result = audioSource.AudioClips.find(keyName);
		if (result == audioSource.AudioClips.end())
		{
			NATIVE_WARN("Audio {} clip not exist!", keyName);
			return;
		}

		auto& clip = result->second;

		audioEngine->LoadClip(clip->FilePath, clip->B3D, clip->IsLooping, clip->BStream);

		int id = audioEngine->PlayClip(clip->FilePath, clip->WorldPos, clip->Volume);
		clip->Channel = id;
	}

	void AudioSystem::StopClip(const std::string& keyName, AudioSourceComponent& audioSource, AudioEngine* audioEngine)
	{
		auto result = audioSource.AudioClips.find(keyName);
		if (result == audioSource.AudioClips.end())
		{
			NATIVE_WARN("Audio {} clip not exist!", keyName);
			return;
		}

		auto& clip = result->second;
		audioEngine->StopClip(clip->Channel);
	}

	bool AudioSystem::AddClip(AudioSourceComponent& audioSource, Ref<AudioClip> clip)
	{
		auto result = audioSource.AudioClips.find(clip->ClipName);
		if (result != audioSource.AudioClips.end())
		{
			NATIVE_WARN("Audio clip already exist!");
			return false;
		}

		if (audioSource.AudioClips.size() == 0)
		{
			clip->isDefaultClip = true;
		}

		audioSource.AudioClips[clip->ClipName] = clip;
		return true;
	}

	bool AudioSystem::DeleteClip(AudioSourceComponent& audioSource, Ref<AudioClip> clip)
	{
		return audioSource.AudioClips.erase(clip->ClipName);
	}

	bool AudioSystem::RemoveClip(AudioSourceComponent& audioSource, const std::string& keyName)
	{
		return audioSource.AudioClips.erase(keyName);
	}

	bool AudioSystem::RenameClip(AudioSourceComponent& audioSource, const std::string& keyName, const std::string& newName)
	{
		auto& searchNewName = audioSource.AudioClips.find(newName);
		if (searchNewName != audioSource.AudioClips.end())
		{
			NATIVE_WARN("Audio clip <{}> already exist!", newName);
			return false;
		}

		auto& search = audioSource.AudioClips.find(keyName);
		if (search != audioSource.AudioClips.end())
		{
			Ref<AudioClip> clipRef = search->second;

			if (RemoveClip(audioSource, keyName))
			{
				clipRef->ClipName = newName;
				audioSource.AudioClips[newName] = clipRef;
				return true;
			}
		}

		return false;
	}

	void AudioSystem::ReloadAllClips(AudioSourceComponent& audioSource, AudioEngine* audioEngine)
	{
		for (auto pair : audioSource.AudioClips)
		{
			auto& [key, clip] = pair;

			audioEngine->LoadClip(clip->FilePath, clip->B3D, clip->IsLooping, clip->BStream);
		}
	}

	void AudioSystem::DebugPlay(Ref<AudioClip> clip, AudioEngine* audioEngine)
	{
		if (!audioEngine) { return; }

		for (auto pair : audioEngine->m_ChannelsMap)
		{
			auto& [key, channel] = pair;
			audioEngine->StopClip(key);
		}

		auto c_result = audioEngine->m_ChannelsMap.find(clip->Channel);
		if (c_result != audioEngine->m_ChannelsMap.end())
		{
			audioEngine->m_ChannelsMap.erase(c_result->first);
		}

		auto s_result = audioEngine->m_SoundsMap.find(clip->FilePath);
		if (s_result != audioEngine->m_SoundsMap.end())
		{
			audioEngine->m_SoundsMap.erase(s_result->first);
		}

		audioEngine->LoadClip(clip->FilePath, clip->B3D, clip->IsLooping, clip->BStream);
		audioEngine->PlayClip(clip->FilePath, clip->WorldPos, clip->Volume);
	}

	void AudioSystem::DebugStop(Ref<AudioClip> clip, AudioEngine* audioEngine)
	{
		if (!audioEngine) { return; }

		for (auto pair : audioEngine->m_ChannelsMap)
		{
			auto& [key, channel] = pair;
			audioEngine->StopClip(key);
		}

		auto c_result = audioEngine->m_ChannelsMap.find(clip->Channel);
		if (c_result != audioEngine->m_ChannelsMap.end())
		{
			audioEngine->m_ChannelsMap.erase(c_result->first);
		}

		auto s_result = audioEngine->m_SoundsMap.find(clip->FilePath);
		if (s_result != audioEngine->m_SoundsMap.end())
		{
			audioEngine->m_SoundsMap.erase(s_result->first);
		}
	}


}