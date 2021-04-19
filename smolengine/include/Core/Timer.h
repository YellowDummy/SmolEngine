#pragma once
#include <chrono>
#include <string>
#include <fstream>
#include <algorithm>

class Timer
{
public:

	Timer() = default;

	Timer(const char* TimerName)
		:m_TimerName(TimerName), 
		m_Stopped(false), m_ResultTime(0)
	{

	}

	~Timer()
	{
		if (!m_Stopped) { StopTimer("Destructor: Forced Shutdown"); }
	}

	void StartTimer()
	{
		m_StartTime = std::chrono::high_resolution_clock::now();
	}

	float GetTimeInSeconds()
	{
		auto recordEndTime = std::chrono::high_resolution_clock::now();

		auto startTime = std::chrono::time_point_cast<std::chrono::seconds>(m_StartTime).time_since_epoch().count();
		auto endTime = std::chrono::time_point_cast<std::chrono::seconds>(recordEndTime).time_since_epoch().count();

		return static_cast<float>(endTime - startTime);
	}

	size_t GetTimeInMiliseconds()
	{
		auto recordEndTime = std::chrono::high_resolution_clock::now();

		unsigned long long startTime = std::chrono::time_point_cast<std::chrono::milliseconds>(m_StartTime).time_since_epoch().count();
		unsigned long long endTime = std::chrono::time_point_cast<std::chrono::milliseconds>(recordEndTime).time_since_epoch().count();

		return m_ResultTime = endTime - startTime;
	}

	void StopTimer(const char* shutdownState = "No Errors")
	{
		auto recordEndTime = std::chrono::high_resolution_clock::now();

		{
			unsigned long long startTime = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTime).time_since_epoch().count();
			unsigned long long endTime = std::chrono::time_point_cast<std::chrono::microseconds>(recordEndTime).time_since_epoch().count();

			m_ResultTime = endTime - startTime;
			m_Stopped = true;
		}
	}

private:

	size_t                                               m_ResultTime;
	std::chrono::time_point<std::chrono::steady_clock>   m_StartTime;
	const char*                                          m_TimerName = "";
	bool                                                 m_Stopped = false;
};


