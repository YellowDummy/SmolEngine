#include "stdafx.h"
#include "SLog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace SmolEngine 
{
	std::shared_ptr<spdlog::logger> SLog::s_NativeLogger;
	std::shared_ptr<spdlog::logger> SLog::s_ClientLogger;

	void SLog::InitLog()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_NativeLogger = spdlog::stdout_color_mt("SmolEngine");
		s_NativeLogger->set_level(spdlog::level::trace);

	    s_ClientLogger = spdlog::stdout_color_mt("Client");
		s_ClientLogger->set_level(spdlog::level::trace);
	}
}
