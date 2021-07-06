#pragma once
#include "Common/Core.h"
extern "C++"
{
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
}

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	class SLog
	{
	public:

		SLog();
		~SLog();

		static void InitLog();
		static spdlog::logger* GetNativeLogger();

	private:
		
		static SLog*                    s_Instance;
		std::shared_ptr<spdlog::logger> m_NativeLogger = nullptr;
	};
}

#ifdef FROSTIUM_SMOLENGINE_IMPL
#define NATIVE_ERROR(...) ::SmolEngine::SLog::GetNativeLogger()->error(__VA_ARGS__)
#define NATIVE_WARN(...)  ::SmolEngine::SLog::GetNativeLogger()->warn(__VA_ARGS__)
#define NATIVE_INFO(...)  ::SmolEngine::SLog::GetNativeLogger()->trace(__VA_ARGS__)
#else
#define NATIVE_ERROR(...) ::Frostium::SLog::GetNativeLogger()->error(__VA_ARGS__)
#define NATIVE_WARN(...)  ::Frostium::SLog::GetNativeLogger()->warn(__VA_ARGS__)
#define NATIVE_INFO(...)  ::Frostium::SLog::GetNativeLogger()->trace(__VA_ARGS__)
#endif



