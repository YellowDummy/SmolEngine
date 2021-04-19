#pragma once

#include "Core/Core.h"

#include <string>
#include <map>
#include <vector>
#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <glm/glm.hpp>

namespace SmolEngine
{
	class AudioEngine
	{
	public:

		AudioEngine();
		~AudioEngine();

		void Init();
		void Shutdown();
		void Update();
		void Reset();

		void StopClip(int channelID);
		void SetClipPosition(int channelID, const glm::vec3& pos);
		void SetClipVolume(int channelID, float volume);
		void SetEventParameter(const std::string& eventKeyName, const std::string& parameterName, float fValue);

		void GetEventParameter(const std::string& eventKeyName, const std::string& parameterName, float* parameter);
		void LoadClip(const std::string& fileName, bool b3d, bool isLooping, bool bStream);
		void DeleteClip(const std::string& keyName);
		int PlayClip(const std::string& filePath, const glm::vec3& wordPos, float volume);

		void LoadBank(const std::string& filePath, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
		void LoadEvent(const std::string& filePath);
		void PlayEvent(const std::string& keyName);
		void StopEvent(const std::string& keyName, bool immediate);

		bool IsEventPlaying(const std::string& keyName) const;
		int ErrorCheck(FMOD_RESULT result) const;
		static AudioEngine* GetAudioEngine() { return s_Instance; }

	private:

		float DbToVolume(float db);

	private:
		FMOD::System*                                                m_System = nullptr;
		FMOD::Studio::System*                                        m_StudioSystem = nullptr;
		bool                                                         m_IsInitialized = false;
		int                                                          m_NextChannelId = 0;
		static AudioEngine*                                          s_Instance;

		std::map<std::string, FMOD::Studio::EventInstance*>          m_EventsMap;
		std::map<std::string, FMOD::Sound*>                          m_SoundsMap;
		std::map<int, FMOD::Channel*>                                m_ChannelsMap;
		std::map<std::string, FMOD::Studio::Bank*>                   m_BanksMap;

		std::vector<std::map<int, FMOD::Channel*>::iterator>         m_InactiveChannels;

	private:

		friend class AudioSource;
		friend class AudioSystem;
	};
}