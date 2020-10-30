#pragma once
#include "Core/Core.h"

#include <string>

namespace SmolEngine
{
	struct Animation2DComponent;

	struct AnimationClip;

	///

	class Animation2DSystem
	{
	public:

		Animation2DSystem() = default;

		/// 

		static void Play(const std::string& clipName, Animation2DComponent& anim);

		static void Stop(const std::string& clipName, Animation2DComponent& anim);

		static void OnAwake(Animation2DComponent& anim);

		static void OnReset(Animation2DComponent& anim);

		/// Update-Loop

		static void Update(const Animation2DComponent& anim);

		/// Reset

		static void Reset(Ref<AnimationClip> anim);

		static void ResetAllFrames(const Animation2DComponent& anim);

		/// Load

		static bool LoadClip(Animation2DComponent& anim, const std::string& filePath);

		///

		static bool RemoveClip(Animation2DComponent& anim, const std::string& keyName);

		static bool RenameClip(Animation2DComponent& anim, const std::string& keyName, const std::string& newName);

		/// Debug

		static void DebugPlay(AnimationClip* anim);

		static void DebugUpdate(AnimationClip* anim);

		static void DebugStop(AnimationClip* anim);

		static void DebugReset(AnimationClip* anim);

		static void DebugResetAllFrames(AnimationClip* anim);

	private:

		friend class EditorLayer;

		friend class WorldAdmin;

	};
}