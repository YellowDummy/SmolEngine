#include "stdafx.h"
#include "Animation2DSystem.h"

#include "Core/ECS/Components/Animation2DComponent.h"
#include "Core/Animation/AnimationClip.h"

#include "Core/ECS/Scene.h"

#include "Core/SLog.h"

namespace SmolEngine
{
	void Animation2DSystem::Play(const std::string& clipName, Animation2DComponent& anim)
	{
		const auto& search = anim.m_Clips.find(clipName);
		if (search == anim.m_Clips.end())
		{
			return;
		}

		if (anim.CurrentClip)
		{
			anim.CurrentClip = nullptr;
			Reset(search->second);
		}

		auto clip = search->second;
		anim.CurrentClip = search->second;

		clip->m_CurrentIndex = 0;

		if (clip->m_Frames.size() == 0) { return; }
		if (clip->m_Frames[clip->m_CurrentIndex]->Texture == nullptr) { return; }

		clip->m_CurrentFrameKey = clip->m_Frames[clip->m_CurrentIndex];
		clip->m_CurrentTexture = clip->m_CurrentFrameKey->Texture;

		clip->m_IsActive = true;
		clip->m_Timer.StartTimer();

	}

	void Animation2DSystem::Stop(const std::string& clipName, Animation2DComponent& anim)
	{
		const auto& search = anim.m_Clips.find(clipName);
		if (search == anim.m_Clips.end())
		{
			return;
		}

		anim.CurrentClip = nullptr;
		Reset(search->second);
	}

	void Animation2DSystem::OnAwake(Animation2DComponent& anim)
	{
		OnReset(anim);

		for (const auto& pair : anim.m_Clips)
		{
			auto& [key, clip] = pair;

			if (clip->m_IsDefaultClip)
			{
				Play(clip->m_ClipName, anim);
				continue;
			}

			Reset(clip);
		}
	}

	void Animation2DSystem::OnReset(Animation2DComponent& anim)
	{
		anim.CurrentClip = nullptr;

		for (const auto& pair : anim.m_Clips)
		{
			auto& [key, clip] = pair;

			Reset(clip);
		}
	}

	void Animation2DSystem::Update(const Animation2DComponent& anim)
	{
		if (anim.CurrentClip) 
		{
			auto animation = anim.CurrentClip;

			auto currentTexture = animation->m_Frames[animation->m_CurrentIndex];
			if (animation->m_IsActive && currentTexture)
			{
				if (animation->m_Timer.GetTimeInMiliseconds() > currentTexture->Speed)
				{
					animation->m_CurrentIndex++;
					if (animation->m_CurrentIndex == animation->m_Frames.size()) { animation->m_CurrentIndex = 0; }
					animation->m_CurrentFrameKey = animation->m_Frames[animation->m_CurrentIndex];
					auto newTexture = animation->m_CurrentFrameKey->Texture;
					if (!newTexture)
					{
						return;
					}

					animation->m_CurrentTexture = newTexture;
					animation->m_Timer.StopTimer();
					animation->m_Timer.StartTimer();
				}
			}
		}
	}

	void Animation2DSystem::Reset(Ref<AnimationClip> anim)
	{
		anim->m_IsActive = false;
		anim->m_CurrentIndex = 0;
		anim->m_CurrentFrameKey = nullptr;
		anim->m_CurrentTexture = nullptr;
		anim->m_Timer.StopTimer();
	}

	void Animation2DSystem::ResetAllFrames(const Animation2DComponent& anim)
	{
		for (auto& pair : anim.m_Clips)
		{
			const auto& [key, value] = pair;

			Reset(value);
			value->m_Frames.clear();
		}
	}

	bool Animation2DSystem::LoadClip(Animation2DComponent& anim, const std::string& filePath)
	{
		std::ifstream file(filePath);
		std::stringstream storage;

		if (!file)
		{
			CONSOLE_ERROR(std::string("Animation2DController: Could not open the file!"));
			return false;
		}

		// Reading file

		storage << file.rdbuf();
		file.close();

		// Creating new clip

		auto& Clip = std::make_shared<AnimationClip>();

		if (anim.m_Clips.size() == 0)
		{
			Clip->m_IsDefaultClip = true;
		}

		// Loading Clip Data

		{
			cereal::JSONInputArchive dataInput{ storage };
			dataInput(Clip->m_Frames, Clip->m_ClipName);
		}

		const auto assetMap = Scene::GetScene()->GetAssetMap();

		// Loading Textures

		for (auto& pair: Clip->m_Frames)
		{
			auto& [value, frame] = pair;

			auto result = assetMap.find(frame->FileName);

			if (result != assetMap.end())
			{
				frame->Texture = Texture2D::Create(result->second);
				continue;
			}

			if (Scene::GetScene()->PathCheck(frame->TexturePath, frame->FileName))
			{
				frame->Texture = Texture2D::Create(frame->TexturePath);
			}
		}

		// Checking if clip already exists

		auto result = anim.m_Clips.find(Clip->m_ClipName);
		if (result == anim.m_Clips.end())
		{
			anim.m_Clips[Clip->m_ClipName] = Clip;

			NATIVE_INFO("Animation2DController: Clip Loaded");
			return true;
		}

		CONSOLE_WARN("Animation2D: Clip already exists");
		return false;
	}

	bool Animation2DSystem::RemoveClip(Animation2DComponent& anim, const std::string& keyName)
	{
		return anim.m_Clips.erase(keyName);
	}

	bool Animation2DSystem::RenameClip(Animation2DComponent& anim, const std::string& keyName, const std::string& newName)
	{
		auto& searchNewName = anim.m_Clips.find(newName);
		if (searchNewName != anim.m_Clips.end())
		{
			NATIVE_WARN("Animation2D: clip <{}> already exist!", newName);
			return false;
		}

		auto& search = anim.m_Clips.find(keyName);
		if (search != anim.m_Clips.end())
		{
			Ref<AnimationClip> clipRef = search->second;

			if (anim.CurrentClip)
			{
				Stop(keyName, anim);
			}

			if (RemoveClip(anim, keyName))
			{
				clipRef->m_ClipName = newName;
				anim.m_Clips[newName] = clipRef;
				return true;
			}
		}

		return false;
	}

	void Animation2DSystem::DebugPlay(AnimationClip* clip)
	{
		clip->m_CurrentIndex = 0;

		if (clip->m_Frames.size() == 0) { return; }
		if (clip->m_Frames[clip->m_CurrentIndex]->Texture == nullptr) { return; }

		clip->m_CurrentFrameKey = clip->m_Frames[clip->m_CurrentIndex];
		clip->m_CurrentTexture = clip->m_CurrentFrameKey->Texture;

		clip->m_IsActive = true;
		clip->m_Timer.StartTimer();
	}

	void Animation2DSystem::DebugUpdate(AnimationClip* animation)
	{
		auto currentTexture = animation->m_Frames[animation->m_CurrentIndex];
		if (animation->m_IsActive && currentTexture)
		{
			if (animation->m_Timer.GetTimeInMiliseconds() > currentTexture->Speed)
			{
				animation->m_CurrentIndex++;
				if (animation->m_CurrentIndex == animation->m_Frames.size()) { animation->m_CurrentIndex = 0; }
				animation->m_CurrentFrameKey = animation->m_Frames[animation->m_CurrentIndex];
				auto newTexture = animation->m_CurrentFrameKey->Texture;
				if (!newTexture)
				{
					return;
				}

				animation->m_CurrentTexture = newTexture;
				animation->m_Timer.StopTimer();
				animation->m_Timer.StartTimer();
			}
		}
	}

	void Animation2DSystem::DebugStop(AnimationClip* anim)
	{
		DebugReset(anim);
	}

	void Animation2DSystem::DebugReset(AnimationClip* anim)
	{
		anim->m_IsActive = false;
		anim->m_CurrentIndex = 0;
		anim->m_CurrentFrameKey = nullptr;
		anim->m_CurrentTexture = nullptr;
		anim->m_Timer.StopTimer();
	}

	void Animation2DSystem::DebugResetAllFrames(AnimationClip* anim)
	{
		DebugReset(anim);
		anim->m_Frames.clear();
	}
}