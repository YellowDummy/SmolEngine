#include "stdafx.h"
#include "Animation2DController.h"
#include "Core/ECS/Scene.h"
#include "Core/ImGui/EditorConsole.h"
#include "Core/Animation/Animation2D.h"

#include <cereal/archives/json.hpp>

namespace SmolEngine
{
	Animation2DController::Animation2DController()
	{

	}

	void Animation2DController::OnAwake()
	{
		bool found = false;
		for (auto pair: m_Clips)
		{
			auto& [key, clip] = pair;
			if (clip->IsDefaultClip && m_PlayOnAwake)
			{
				m_CurrentClip = clip;
				found = true;
				break;
			}
		}

		if (found)
		{
			Play();
			m_State = Animation2DControllerState::PlayState;
			return;
		}

		m_State = Animation2DControllerState::SleepState;
	}

	void Animation2DController::Play()
	{
		if (m_CurrentClip != nullptr)
		{
			m_CurrentClip->Clip->Play();
		}
	}

	void Animation2DController::Reset()
	{
		if (m_CurrentClip != nullptr && m_State == Animation2DControllerState::PlayState)
		{
			m_CurrentClip->Clip->Reset();
			m_State = Animation2DControllerState::SleepState;
			m_CurrentClip = nullptr;
		}
	}

	void Animation2DController::Update()
	{
		if (m_CurrentClip != nullptr && m_State == Animation2DControllerState::PlayState)
		{
			m_CurrentClip->Clip->Update();
		}
	}

	void Animation2DController::FindState()
	{
		
	}

	void Animation2DController::PlayClip(const std::string& clipName)
	{
		auto result = m_Clips.find(clipName);
		if (result != m_Clips.end())
		{
			if (m_CurrentClip != nullptr)
			{
				m_CurrentClip->Clip->Reset();
				m_CurrentClip = nullptr;
			}

			m_CurrentClip = result->second;
			m_State = Animation2DControllerState::PlayState;
			m_CurrentClip->Clip->Play();
			return;
		}

		CONSOLE_WARN(std::string("Clip " + clipName + std::string(" not found")));
	}

	void Animation2DController::ReloadTextures()
	{
		auto scene = Scene::GetScene();

		for (auto pair: m_Clips)
		{
			auto& [key, clip] = pair;

			for (auto pairW: clip->Clip->m_Frames)
			{
				auto& [key, frame] = pairW;

				if (scene->PathCheck(frame->TexturePath, frame->FileName))
				{
					frame->Texture = Texture2D::Create(frame->TexturePath);
				}
			}
		}
	}

	void Animation2DController::LoadClip(const std::string& filePath)
	{
		std::ifstream file(filePath);
		std::stringstream storage;

		if (file)
		{
			storage << file.rdbuf();
			file.close();
		}
		else
		{
			CONSOLE_ERROR(std::string("Animation2DController: Could not open the file!"));
			return;
		}

		auto animationClip = std::make_shared<AnimationClip>();
		animationClip->ClipIndex = m_Clips.size();
		if (m_Clips.size() == 0)
		{
			animationClip->IsDefaultClip = true;
		}

		animationClip->Clip = std::make_unique<Animation2D>();

		auto& Clip = animationClip->Clip;

		{
			cereal::JSONInputArchive sceneDataInput{ storage };
			sceneDataInput(Clip->m_Frames, Clip->m_ClipName);
		}

		auto scene = Scene::GetScene();

		for (auto pair : Clip->m_Frames)
		{
			auto& [index, frame] = pair;

			if (scene->PathCheck(frame->TexturePath, frame->FileName))
			{
				frame->Texture = Texture2D::Create(frame->TexturePath);
			}
		}

		auto result = m_Clips.find(Clip->m_ClipName);
		if (result == m_Clips.end())
		{
			m_Clips[Clip->m_ClipName] = animationClip;
			NATIVE_INFO("Animation2DController: Clip Loaded");
			return;
		}

		CONSOLE_WARN("Animation2DController: Clip already exists");
	}

}