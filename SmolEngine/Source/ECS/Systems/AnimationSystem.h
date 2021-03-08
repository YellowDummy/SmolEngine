#pragma once
#include "Core/Core.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <entt.hpp>

namespace SmolEngine
{
	struct AnimatorComponent;
	struct SkeletalAnimationClip;

	class AnimationSystem
	{
	public:

		static void OnSceneStart(entt::registry& registry);

		static void OnSceneEnd(entt::registry& registry);

		static void Update(entt::registry& registry);

	private:

		static void UpdateAnimator(AnimatorComponent* comp);

		// Helpers

		static void CalcInterpolatedScaling(float animationTime, SkeletalAnimationClip* clip, glm::vec3& outScale);

		static void CalcInterpolatedPosition(float animationTime, SkeletalAnimationClip* clip, glm::vec3& outPos);

		static void CalcInterpolatedRotation(float animationTime, SkeletalAnimationClip* clip, glm::quat& outRot);
	};
}