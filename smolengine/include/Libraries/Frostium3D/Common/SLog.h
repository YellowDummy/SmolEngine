#pragma once
#include "Common/Core.h"

#include <functional>
#include <sstream>

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
	enum class LogType
	{
		Info, 
		Warning, 
		Error
	};

	class SLog
	{
	public:
		SLog(const std::string& name = "Frostium");
		~SLog();

		void                    SetOnPrintCallback(const std::function<void(const std::string&&, LogType)>& callback);
		static spdlog::logger*  GetLogger();

	public:
		inline static SLog*                               s_Instance = nullptr;
		std::function<void(const std::string&&, LogType)> m_Callback = nullptr;
		std::shared_ptr<spdlog::logger>                   m_Logger = nullptr;
	};

	template<typename FormatString, typename... Args>
	void NATIVE_ERROR(FormatString&& fmt, Args&& ...args)
	{
		SLog::GetLogger()->error(std::forward<FormatString>(fmt), std::forward<Args>(args)...);
		auto& callback = SLog::s_Instance->m_Callback;
		if (callback != nullptr)
		{
			spdlog::memory_buf_t buf;
			fmt::format_to(buf, fmt, args...);
			auto result = std::string(buf.data(), buf.size());
			callback(std::forward<std::string>(result), LogType::Error);
		}
	}

	template<typename FormatString, typename... Args>
	void NATIVE_INFO(FormatString&& fmt, Args&& ...args)
	{
		SLog::GetLogger()->trace(std::forward<FormatString>(fmt), std::forward<Args>(args)...);
		auto& callback = SLog::s_Instance->m_Callback;
		if (callback != nullptr)
		{
			spdlog::memory_buf_t buf;
			fmt::format_to(buf, fmt, args...);
			auto result = std::string(buf.data(), buf.size());
			callback(std::forward<std::string>(result), LogType::Info);
		}
	}

	template<typename FormatString, typename... Args>
	void NATIVE_WARN(FormatString&& fmt, Args&& ...args)
	{
		SLog::GetLogger()->warn(std::forward<FormatString>(fmt), std::forward<Args>(args)...);
		auto& callback = SLog::s_Instance->m_Callback;
		if (callback != nullptr)
		{
			spdlog::memory_buf_t buf;
			fmt::format_to(buf, fmt, args...);
			auto result = std::string(buf.data(), buf.size());
			callback(std::forward<std::string>(result), LogType::Warning);
		}
	}
}



