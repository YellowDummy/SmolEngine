#pragma once
#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace SmolEngine 
{
	class SMOL_ENGINE_API SLog
	{
	public:

		static void InitLog();

		inline static std::shared_ptr<spdlog::logger>& GetNativeLogger() { return s_NativeLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:

		static std::shared_ptr<spdlog::logger> s_NativeLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

//#define INIT_LOG ::SmolEngine::SLog::InitLog()

//---------------------------------------------Native Logger-------------------------------------------------------------------
#define NATIVE_ERROR(...) ::SmolEngine::SLog::GetNativeLogger()->error(__VA_ARGS__)
#define NATIVE_WARN(...)  ::SmolEngine::SLog::GetNativeLogger()->warn(__VA_ARGS__)
#define NATIVE_INFO(...)  ::SmolEngine::SLog::GetNativeLogger()->info(__VA_ARGS__)
//---------------------------------------------Native Logger-------------------------------------------------------------------

//---------------------------------------------Client Logger-------------------------------------------------------------------
#define S_ERROR(...) ::SmolEngine::SLog::GetClientLogger()->error(__VA_ARGS__)
#define S_WARN(...)  ::SmolEngine::SLog::GetClientLogger()->warn(__VA_ARGS__)
#define S_INFO(...)  ::SmolEngine::SLog::GetClientLogger()->info(__VA_ARGS__)
//---------------------------------------------Client Logger-------------------------------------------------------------------

