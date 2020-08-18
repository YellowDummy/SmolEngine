#pragma once

namespace SmolEngine
{
	struct DeltaTime
	{
		DeltaTime(float time = 0.0f)
			:m_Time(time)
		{
		}

		inline float GetTimeSeconds() const { return m_Time; }
		inline float GetTimeMiliseconds() const { return m_Time * 1000; }
		operator float() const { return m_Time; }

	private:
		float m_Time;
	};
}
