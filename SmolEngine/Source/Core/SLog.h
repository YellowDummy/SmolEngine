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
		inline static std::shared_ptr<spdlog::logger>& GetEditorLogger() { return s_EditorLogger; }

	private:

		static std::shared_ptr<spdlog::logger> s_NativeLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
		static std::shared_ptr<spdlog::logger> s_EditorLogger;
	};
}

//#define INIT_LOG ::SmolEngine::SLog::InitLog()

//---------------------------------------------Native Logger-------------------------------------------------------------------

#define NATIVE_ERROR(...) ::SmolEngine::SLog::GetNativeLogger()->error(__VA_ARGS__)
#define NATIVE_WARN(...)  ::SmolEngine::SLog::GetNativeLogger()->warn(__VA_ARGS__)
#define NATIVE_INFO(...)  ::SmolEngine::SLog::GetNativeLogger()->trace(__VA_ARGS__)

//---------------------------------------------Native Logger-------------------------------------------------------------------



//---------------------------------------------Client Logger-------------------------------------------------------------------

#define CLIENT_ERROR(...)      ::SmolEngine::SLog::GetClientLogger()->error(__VA_ARGS__)
#define CLIENT_WARN(...)       ::SmolEngine::SLog::GetClientLogger()->warn(__VA_ARGS__)
#define CLIENT_INFO(...)       ::SmolEngine::SLog::GetClientLogger()->trace(__VA_ARGS__)

//---------------------------------------------Client Logger-------------------------------------------------------------------


//----------------------------------------------Editor Logger-------------------------------------------------------------------

#define EDITOR_ERROR(...)      ::SmolEngine::SLog::GetEditorLogger()->error(__VA_ARGS__)
#define EDITOR_WARN(...)       ::SmolEngine::SLog::GetEditorLogger()->warn(__VA_ARGS__)
#define EDITOR_INFO(...)       ::SmolEngine::SLog::GetEditorLogger()->trace(__VA_ARGS__)

//----------------------------------------------Editor Logger-------------------------------------------------------------------


