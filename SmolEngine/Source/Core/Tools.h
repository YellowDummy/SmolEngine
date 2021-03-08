#pragma once

//---------------STD-INCLUDES------------------//
#include <chrono>
#include <string>
#include <fstream>
#include <algorithm>
//---------------------------------------------//


//-------------SPDLOG-INCLUDES---------------==//
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
//---------------------------------------------//

class ToolLogger 
{
public:

	ToolLogger(const char* LoggerName)
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_ToolLogger = spdlog::stdout_color_mt(LoggerName);
		s_ToolLogger->set_level(spdlog::level::trace);
	}

	inline std::shared_ptr<spdlog::logger>& GetLogger()
	{
		return s_ToolLogger;
	}

private:
	std::shared_ptr<spdlog::logger> s_ToolLogger;
};

class ToolTimer
{
public:

	size_t m_resultTime;

	ToolTimer() = default;

	ToolTimer(const char* TimerName)
		:m_TimerName(TimerName), 
		m_Stopped(false), m_resultTime(0), 
		m_Logger(new ToolLogger(TimerName))
	{

	}

	~ToolTimer()
	{
		if (!m_Stopped) { StopTimer("Destructor: Forced Shutdown"); }
		if (m_Logger != nullptr)
		{
			delete m_Logger;
		}
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

		return m_resultTime = endTime - startTime;
	}

	void StopTimer(const char* shutdownState = "No Errors")
	{
		auto recordEndTime = std::chrono::high_resolution_clock::now();

		{
			unsigned long long startTime = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTime).time_since_epoch().count();
			unsigned long long endTime = std::chrono::time_point_cast<std::chrono::microseconds>(recordEndTime).time_since_epoch().count();

			m_resultTime = endTime - startTime;
			m_Stopped = true;
		}

		if (m_Logger != nullptr)
		{
			m_Logger->GetLogger()->warn("{0} | Execution Time : {1} ms | Shutdown State : {2}", m_TimerName, m_resultTime, shutdownState);
		}
	}

private:

	ToolLogger*                                          m_Logger = nullptr;
	std::chrono::time_point<std::chrono::steady_clock>   m_StartTime;
	const char*                                          m_TimerName = "";
	bool                                                 m_Stopped = false;
};


