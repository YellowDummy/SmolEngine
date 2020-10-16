#pragma once
#include "Core/Core.h"

#include <string>
#include <unordered_map>
#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>

namespace SmolEngine
{
	class Animation2D;

	struct AnimationClip
	{
		AnimationClip() = default;

		Ref<Animation2D> Clip = nullptr;

		uint32_t ClipIndex = 0;

		bool IsDefaultClip = false;

	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(ClipIndex, Clip, IsDefaultClip);
		}
	};

	enum class Animation2DControllerState: uint16_t
	{
		None = 0,
		PlayState, SleepState
	};

	class Animation2DController
	{
	public:

		Animation2DController();

		///

		void OnAwake();

		void Play();

		void Reset();

		void Update();

		void FindState();

		void PlayClip(const std::string& clipName);

	private:

		void ReloadTextures();

		void LoadClip(const std::string& filePath);

	private:

		std::unordered_map<std::string, Ref<AnimationClip>> m_Clips;

		Ref<AnimationClip> m_CurrentClip = nullptr;

		bool m_PlayOnAwake = true;

		Animation2DControllerState m_State = Animation2DControllerState::None;

	private:

		friend class cereal::access;
		friend class EditorLayer;
		friend class Scene;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(m_Clips, m_PlayOnAwake);
		}
	};
}