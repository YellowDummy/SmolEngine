#pragma once
#include "Core/Core.h"

#include <string>
#include <entt/entt.hpp>

namespace SmolEngine
{
	struct Animation2DComponent;
	struct AnimationClip2D;

	class Animation2DSystem
	{
	public:

		static void Play(const std::string& clipName, Animation2DComponent& anim);
		static void Stop(const std::string& clipName, Animation2DComponent& anim);

		static void OnAwake(entt::registry& registry);
		static void OnReset(entt::registry& registry);

		static void Update(entt::registry& registry);
		static void Reset(Ref<AnimationClip2D> anim);
		static void ResetAllFrames(const Animation2DComponent& anim);

		static bool LoadClip(Animation2DComponent& anim, const std::string& filePath);
		static bool RemoveClip(Animation2DComponent& anim, const std::string& keyName);
		static bool RenameClip(Animation2DComponent& anim, const std::string& keyName, const std::string& newName);

		static void DebugPlay(AnimationClip2D* anim);
		static void DebugUpdate(AnimationClip2D* anim);
		static void DebugStop(AnimationClip2D* anim);
		static void DebugReset(AnimationClip2D* anim);
		static void DebugResetAllFrames(AnimationClip2D* anim);

	private:

		friend class EditorLayer;
		friend class WorldAdmin;

	};
}