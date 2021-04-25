#pragma once
#include "Common/Core.h"
extern "C++"
{
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
}

namespace Frostium
{
	class SLog
	{
	public:

		SLog();
		~SLog();

		static void InitLog();
		static spdlog::logger* GetNativeLogger();

	private:
		
		static SLog* s_Instance;
		std::shared_ptr<spdlog::logger> m_NativeLogger = nullptr;
	};
}

#define NATIVE_ERROR(...) ::Frostium::SLog::GetNativeLogger()->error(__VA_ARGS__)
#define NATIVE_WARN(...)  ::Frostium::SLog::GetNativeLogger()->warn(__VA_ARGS__)
#define NATIVE_INFO(...)  ::Frostium::SLog::GetNativeLogger()->trace(__VA_ARGS__)



