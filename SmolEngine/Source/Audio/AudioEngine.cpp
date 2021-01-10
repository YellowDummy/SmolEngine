#include "stdafx.h"
#include "AudioEngine.h"
#include "ImGui/EditorConsole.h"

namespace SmolEngine
{
	AudioEngine* AudioEngine::s_Instance = new AudioEngine();

	AudioEngine::AudioEngine()
	{

	}

	AudioEngine::~AudioEngine()
	{
		ErrorCheck(m_StudioSystem->unloadAll());
		ErrorCheck(m_StudioSystem->release());

		m_IsInitialized = false;
	}

	void AudioEngine::Init()
	{
		m_StudioSystem = nullptr;
		m_System = nullptr;
		m_NextChannelId = 0;

		ErrorCheck(FMOD::Studio::System::create(&m_StudioSystem));
		ErrorCheck(m_StudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));
		ErrorCheck(m_StudioSystem->getCoreSystem(&m_System));

		m_IsInitialized = true;
	}

	void AudioEngine::Shutdown()
	{
		ErrorCheck(m_StudioSystem->unloadAll());
		ErrorCheck(m_StudioSystem->release());

		m_BanksMap.clear();
		m_ChannelsMap.clear();
		m_EventsMap.clear();
		m_SoundsMap.clear();
		m_InactiveChannels.clear();

		m_System = nullptr;
		m_StudioSystem = nullptr;

		if (m_IsInitialized) { m_IsInitialized = false; }
	}

	void AudioEngine::Update()
	{
		if (!m_IsInitialized) { return; }

		for (auto it = m_ChannelsMap.begin(), itEnd = m_ChannelsMap.end(); it != itEnd; ++it)
		{
			bool bIsPlaying = false;
			it->second->isPlaying(&bIsPlaying);
			if (!bIsPlaying)
			{
				m_InactiveChannels.push_back(it);
			}
		}

		if (m_InactiveChannels.size() > 0)
		{
			for (auto& it: m_InactiveChannels)
			{
				m_ChannelsMap.erase(it);
			}

			m_InactiveChannels.clear();
		}

		ErrorCheck(m_StudioSystem->update());
	}

	void AudioEngine::Reset()
	{
		Shutdown();
		Init();
	}

	void AudioEngine::LoadClip(const std::string& filePath, bool b3d, bool isLooping, bool bStream)
	{
		if (!m_IsInitialized) { return; }

		auto result = m_SoundsMap.find(filePath);
		if (result != m_SoundsMap.end())
		{
			CONSOLE_WARN("Sound already exist! Replacing...");
			m_SoundsMap.erase(filePath);
		}

		FMOD_MODE eMode = FMOD_DEFAULT;
		eMode |= b3d ? FMOD_3D : FMOD_2D;
		eMode |= isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

		FMOD::Sound* pSound = nullptr;
		ErrorCheck(m_System->createSound(filePath.c_str(), eMode, nullptr, &pSound));
		if (pSound) 
		{
			m_SoundsMap[filePath] = pSound;
		}
	}

	void AudioEngine::DeleteClip(const std::string& keyName)
	{
		auto result = m_SoundsMap.find(keyName);
		if (result == m_SoundsMap.end())
		{
			CONSOLE_WARN("Sound not found!");
			return;
		}

		ErrorCheck(result->second->release());
		m_SoundsMap.erase(result);
	}

	int AudioEngine::PlayClip(const std::string& filePath, const glm::vec3& wordPos, float volume)
	{
		int channelID = m_NextChannelId++;

		auto result = m_SoundsMap.find(filePath);
		if (result == m_SoundsMap.end())
		{
			CONSOLE_WARN("Sound not found!");
			return 0;
		}

		FMOD::Channel* pChannel = nullptr;
		ErrorCheck(m_System->playSound(result->second, nullptr, true, &pChannel));
		if (pChannel)
		{
			FMOD_MODE mode;
			result->second->getMode(&mode);
			if (mode & FMOD_3D) 
			{
				FMOD_VECTOR position = { wordPos.x, wordPos.y, wordPos.z };
				ErrorCheck(pChannel->set3DAttributes(&position, nullptr));
			}

			ErrorCheck(pChannel->setVolume(DbToVolume(volume)));
			ErrorCheck(pChannel->setPaused(false));
			m_ChannelsMap[channelID] = pChannel;
		}

		return channelID;
	}

	void AudioEngine::StopClip(int channelID)
	{
		auto result = m_ChannelsMap.find(channelID);
		if (result == m_ChannelsMap.end())
		{
			NATIVE_ERROR("Channel not found!");
			return;
		}

		result->second->stop();
	}

	void AudioEngine::SetClipPosition(int channelID, const glm::vec3& pos)
	{
		auto result = m_ChannelsMap.find(channelID);
		if (result == m_ChannelsMap.end())
		{
			NATIVE_WARN("Channel not found!");
			return;
		}

		FMOD_VECTOR position = { pos.x, pos.y, pos.z };
		ErrorCheck(result->second->set3DAttributes(&position, NULL));
	}

	void AudioEngine::SetClipVolume(int channelID, float volume)
	{
		auto result = m_ChannelsMap.find(channelID);
		if (result == m_ChannelsMap.end())
		{
			NATIVE_WARN("Channel not found!");
			return;
		}

		ErrorCheck(result->second->setVolume(DbToVolume(volume)));
	}

	void AudioEngine::LoadBank(const std::string& filePath, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
	{
		auto result = m_BanksMap.find(filePath);
		if (result != m_BanksMap.end())
		{
			NATIVE_WARN("Bank already exist!");
			return;
		}

		FMOD::Studio::Bank* pBank;
		ErrorCheck(m_StudioSystem->loadBankFile(filePath.c_str(), flags, &pBank));
		if (pBank)
		{
			m_BanksMap[filePath] = pBank;
		}
	}

	void AudioEngine::LoadEvent(const std::string& filePath)
	{
		auto result = m_EventsMap.find(filePath);
		if (result != m_EventsMap.end())
		{
			NATIVE_WARN("Event already exist!");
			return;
		}

		FMOD::Studio::EventDescription* pEventDesc = NULL;
		ErrorCheck(m_StudioSystem->getEvent(filePath.c_str(), &pEventDesc));
		if (pEventDesc)
		{
			FMOD::Studio::EventInstance* pEventInst = NULL;
			ErrorCheck(pEventDesc->createInstance(&pEventInst));
			if (pEventInst)
			{
				m_EventsMap[filePath] = pEventInst;
			}
		}
	}

	void AudioEngine::PlayEvent(const std::string& keyName)
	{
		auto result = m_EventsMap.find(keyName);
		if (result == m_EventsMap.end())
		{
			NATIVE_WARN("Event not found!");
			return;
		}

		result->second->start();
	}

	void AudioEngine::StopEvent(const std::string& keyName, bool immediate)
	{
		auto result = m_EventsMap.find(keyName);
		if (result == m_EventsMap.end())
		{
			NATIVE_WARN("Event not found!");
			return;
		}

		FMOD_STUDIO_STOP_MODE eMode;
		eMode = immediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
		result->second->stop(eMode);
	}

	void AudioEngine::GetEventParameter(const std::string& eventKeyName, const std::string& parameterName, float* parameter)
	{
		auto result = m_EventsMap.find(eventKeyName);
		if (result == m_EventsMap.end())
		{
			NATIVE_WARN("Event not found!");
			return;
		}

		ErrorCheck(result->second->getParameterByName(parameterName.c_str(), parameter));
	}

	void AudioEngine::SetEventParameter(const std::string& eventKeyName, const std::string& parameterName, float fValue)
	{
		auto result = m_EventsMap.find(eventKeyName);
		if (result == m_EventsMap.end())
		{
			NATIVE_WARN("Event not found!");
			return;
		}

		ErrorCheck(result->second->setParameterByName(parameterName.c_str(), fValue));
	}

	bool AudioEngine::IsEventPlaying(const std::string& keyName) const
	{
		auto result = m_EventsMap.find(keyName);
		if (result == m_EventsMap.end())
		{
			return false;
		}

		FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
		if (result->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING)
		{
			return true;
		}

		return false;
	}

	int AudioEngine::ErrorCheck(FMOD_RESULT result) const
	{
		if (result != FMOD_OK) 
		{
			NATIVE_ERROR("FMOD ERROR: {}", result);
			return 1;
		}
		return 0;
	}

	float AudioEngine::DbToVolume(float db)
	{
		return powf(10.0f, 0.05f * db);
	}
}