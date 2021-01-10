#include "stdafx.h"
#include "AudioSource.h"

#include "Audio/AudioEngine.h"
#include "Audio/AudioClip.h"
#include "ECS/WorldAdmin.h"
#include "Core/SLog.h"

namespace SmolEngine
{
	AudioSource::AudioSource()
	{
		m_AudioEngine = AudioEngine::GetAudioEngine();
	}

	void AudioSource::OnAwake()
	{
		std::map <std::string, Ref<AudioClip>> buffer;

		for (auto pair : m_AudioClips)
		{
			auto& [key, clip] = pair;

			buffer[clip->ClipName] = clip;
		}

		m_AudioClips.clear();
		m_AudioClips = buffer;

		for (auto pair: m_AudioClips)
		{
			auto& [key, clip] = pair;

			if (clip->isDefaultClip && m_PlayOnAwake)
			{
				PlayClip(key, clip->Volume, clip->IsLooping);
				return;
			}
		}
	}

	void AudioSource::PlayClip(const std::string& keyName, float volume, bool loop)
	{
		auto result = m_AudioClips.find(keyName);
		if (result == m_AudioClips.end())
		{
			NATIVE_WARN("Audio {} clip not exist!", keyName);
			return;
		}

		auto& clip = result->second;

		m_AudioEngine->LoadClip(clip->FilePath, clip->B3D, clip->IsLooping, clip->BStream);

		int id = m_AudioEngine->PlayClip(clip->FilePath, clip->WorldPos, clip->Volume);
		clip->Channel = id;
	}

	void AudioSource::StopClip(const std::string& keyName)
	{
		auto result = m_AudioClips.find(keyName);
		if (result == m_AudioClips.end())
		{
			NATIVE_WARN("Audio {} clip not exist!", keyName);
			return;
		}

		auto& clip = result->second;
		m_AudioEngine->StopClip(clip->Channel);
	}

	void AudioSource::AddClip(Ref<AudioClip> clip)
	{
		auto result = m_AudioClips.find(clip->ClipName);
		if (result != m_AudioClips.end())
		{
			NATIVE_WARN("Audio clip already exist!");
			return;
		}

		if (m_AudioClips.size() == 0)
		{
			clip->isDefaultClip = true;
		}

		m_AudioClips[clip->ClipName] = clip;
	}

	void AudioSource::ReloadAllClips()
	{
		for (auto pair: m_AudioClips)
		{
			auto& [key, clip] = pair;

			m_AudioEngine->LoadClip(clip->FilePath, clip->B3D, clip->IsLooping, clip->BStream);
		}
	}

	void AudioSource::DebugPlay(Ref<AudioClip> clip)
	{
		if (!m_AudioEngine) { return; }

		for (auto pair: m_AudioEngine->m_ChannelsMap)
		{
			auto& [key, channel] = pair;
			m_AudioEngine->StopClip(key);
		}

		auto c_result = m_AudioEngine->m_ChannelsMap.find(clip->Channel);
		if (c_result != m_AudioEngine->m_ChannelsMap.end())
		{
			m_AudioEngine->m_ChannelsMap.erase(c_result->first);
		}

		auto s_result = m_AudioEngine->m_SoundsMap.find(clip->FilePath);
		if (s_result != m_AudioEngine->m_SoundsMap.end())
		{
			m_AudioEngine->m_SoundsMap.erase(s_result->first);
		}

		m_AudioEngine->LoadClip(clip->FilePath, clip->B3D, clip->IsLooping, clip->BStream);
		m_AudioEngine->PlayClip(clip->FilePath, clip->WorldPos, clip->Volume);
	}

	void AudioSource::DebugStop(Ref<AudioClip> clip)
	{
		if (!m_AudioEngine) { return; }

		for (auto pair : m_AudioEngine->m_ChannelsMap)
		{
			auto& [key, channel] = pair;
			m_AudioEngine->StopClip(key);
		}

		auto c_result = m_AudioEngine->m_ChannelsMap.find(clip->Channel);
		if (c_result != m_AudioEngine->m_ChannelsMap.end())
		{
			m_AudioEngine->m_ChannelsMap.erase(c_result->first);
		}

		auto s_result = m_AudioEngine->m_SoundsMap.find(clip->FilePath);
		if (s_result != m_AudioEngine->m_SoundsMap.end())
		{
			m_AudioEngine->m_SoundsMap.erase(s_result->first);
		}
	}
}