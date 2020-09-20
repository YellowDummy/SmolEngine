#include "stdafx.h"
#include "Animation2D.h"

#include "Core/SLog.h"

#include <thread>
#include <chrono>

#include "Core/Renderer/Renderer2D.h"

namespace SmolEngine
{
	Animation2D::Animation2D()
	{

	}

	void Animation2D::Play()
	{
		m_CurrentIndex = 0;

		if(m_Frames.size() == 0) { return; }
		if (m_Frames[m_CurrentIndex]->Texture == nullptr) { return; }

		m_CurrentFrameKey = m_Frames[m_CurrentIndex];
		m_CurrentTexture = m_CurrentFrameKey->Texture;

		m_IsActive = true;
		m_Timer.StartTimer();
		
	}

	void Animation2D::Update()
	{
		auto currentTexture = m_Frames[m_CurrentIndex];
		if (m_IsActive && currentTexture)
		{
			if (m_Timer.GetTimeInMiliseconds() > currentTexture->Speed)
			{
				m_CurrentIndex++;
				if (m_CurrentIndex == m_Frames.size()) { m_CurrentIndex = 0; }
				m_CurrentFrameKey = m_Frames[m_CurrentIndex];
				auto newTexture = m_CurrentFrameKey->Texture;
				if (!newTexture)
				{
					return;
				}

				m_CurrentTexture = newTexture;
				m_Timer.StopTimer();
				m_Timer.StartTimer();
			}
		}
	}

	void Animation2D::ResetAllFrames()
	{
		m_IsActive = false;
		m_CurrentIndex = 0;
		m_CurrentFrameKey = nullptr;
		m_CurrentTexture = nullptr;
		m_Timer.StopTimer();

		m_Frames.clear();
	}

	void Animation2D::Reset()
	{
		m_IsActive = false;
		m_CurrentIndex = 0;
		m_CurrentFrameKey = nullptr;
		m_CurrentTexture = nullptr;
		m_Timer.StopTimer();
	}
}