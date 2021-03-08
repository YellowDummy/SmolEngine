#include "stdafx.h"
#include "AnimationSystem.h"

#include "ECS/Components/AnimatorComponent.h"
#include "Utils/ModelImporter.h"

namespace SmolEngine
{
	void AnimationSystem::OnSceneStart(entt::registry& registry)
	{

	}

	void AnimationSystem::OnSceneEnd(entt::registry& registry)
	{

	}

	void AnimationSystem::Update(entt::registry& registry)
	{
		const auto& view = registry.view<AnimatorComponent>();
		for (const auto& entity : view)
		{
			auto& anim = view.get<AnimatorComponent>(entity);
			UpdateAnimator(&anim);
		}

	}

	void AnimationSystem::UpdateAnimator(AnimatorComponent* comp)
	{
		for (auto& clip : comp->Clips)
		{
			float time = 0.0f;
			float recTime = clip.Timer.GetTimeInSeconds();
			if (time >= (float)clip.Duration)
			{
				clip.Timer.StopTimer();
				clip.Timer.StartTimer();
				recTime = clip.Timer.GetTimeInSeconds();
			}
			time = recTime;

			glm::vec3 position;
			glm::vec3 scale;
			glm::quat rotatrion;
			CalcInterpolatedPosition(time, &clip, position);
			CalcInterpolatedScaling(time, &clip, scale);
			CalcInterpolatedRotation(time, &clip, rotatrion);

			glm::mat4 identity = glm::mat4();
			glm::mat4 translationM = glm::translate(glm::mat4(1.0f), position);
			glm::mat4 scalingM = glm::scale(glm::mat4(1.0), scale);
			glm::mat4 rotationM = glm::toMat4(rotatrion);

			glm::mat4 nodeTransformation = translationM * rotationM * scalingM;
			glm::mat4 globalTransformation = identity * nodeTransformation;

		}
	}

	void AnimationSystem::CalcInterpolatedScaling(float animationTime, SkeletalAnimationClip* clip, glm::vec3& outScale)
	{
		// Find the scaling key just before the current animation time and return the index. 
		uint32_t index = 0;
		for (uint32_t i = 0; i < static_cast<uint32_t>(clip->ScaleKeys.size() - 1); i++)
		{
			if (animationTime < (float)clip->ScaleKeys[i + 1].Time) 
			{
				index = i;
				break;
			}
		}

		uint32_t nextIndex = index + 1;
		float deltaTime = static_cast<float>(clip->ScaleKeys[nextIndex].Time - clip->ScaleKeys[index].Time);
		float factor = (animationTime - (float)clip->ScaleKeys[index].Time) / deltaTime;

		const glm::vec3& start = clip->ScaleKeys[index].Value;
		const glm::vec3& end = clip->ScaleKeys[nextIndex].Value;

		glm::vec3 delta = end - start;
		outScale = start + factor * delta;
	}

	void AnimationSystem::CalcInterpolatedPosition(float animationTime, SkeletalAnimationClip* clip, glm::vec3& outPos)
	{
		uint32_t index = 0;
		for (uint32_t i = 0; i < static_cast<uint32_t>(clip->TransformKeys.size() - 1); i++)
		{
			if (animationTime < (float)clip->TransformKeys[i + 1].Time)
			{
				index = i;
				break;
			}
		}

		uint32_t nextIndex = index + 1;
		float deltaTime = static_cast<float>(clip->TransformKeys[nextIndex].Time - clip->TransformKeys[index].Time);
		float factor = (animationTime - (float)clip->TransformKeys[index].Time) / deltaTime;

		const glm::vec3& start = clip->TransformKeys[index].Value;
		const glm::vec3& end = clip->TransformKeys[nextIndex].Value;

		glm::vec3 delta = end - start;
		outPos = start + factor * delta;
	}

	void AnimationSystem::CalcInterpolatedRotation(float animationTime, SkeletalAnimationClip* clip, glm::quat& outRot)
	{
		uint32_t index = 0;
		for (uint32_t i = 0; i < static_cast<uint32_t>(clip->RotationKeys.size() - 1); i++)
		{
			if (animationTime < (float)clip->RotationKeys[i + 1].Time)
			{
				index = i;
				break;
			}
		}

		uint32_t nextIndex = index + 1;
		float deltaTime = static_cast<float>(clip->RotationKeys[nextIndex].Time - clip->RotationKeys[index].Time);
		float factor = (animationTime - (float)clip->RotationKeys[index].Time) / deltaTime;

		const aiQuaternion start = ModelImporter::quat_cast(clip->RotationKeys[index].Value);
		const aiQuaternion end = ModelImporter::quat_cast(clip->RotationKeys[nextIndex].Value);
		aiQuaternion result;
		aiQuaternion::Interpolate(result, start, end, factor);

		result.Normalize();
		outRot = ModelImporter::quat_cast(result);
	}
}