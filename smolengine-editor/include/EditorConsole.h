#pragma once

#include "Core/Core.h"

#include <vector>
#include <ctime>
#include <sstream>

#include <Frostium3D/Libraries/imgui/imgui.h>
#include <Frostium3D/Common/SLog.h>

namespace SmolEngine
{
	enum class LogLevel: int
	{
		None = 0, 
		Info, 
		Error,
		Warn
	};

	struct Message
	{
		ImVec4        Color = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
		std::string   Text = std::string("");
		LogLevel      Type = LogLevel::Info;
	};

	class EditorConsole
	{
	public:

		EditorConsole() { s_Instance = this; }
		~EditorConsole() { s_Instance = nullptr; }

		void Update(bool& enabled)
		{
			if (enabled)
			{
				if (ImGui::Begin("Console", &enabled))
				{
					ImGui::BeginChild("ConsoleChild");
					{
						ImGui::SetWindowFontScale(0.9f);
						ImGui::SameLine();
						if (ImGui::Button("Clear"))
						{
							m_Messages.clear();
						}

						ImGui::SameLine();
						ImGui::Combo("Filter", &m_CurrentItem, "None\0Info\0Error\0Warn\0\0");

						ImGui::Separator();
						ImGui::BeginChild("Log");
						{
							for (auto& msg : m_Messages)
							{
								if (m_CurrentItem == 0 ? true: m_CurrentItem == (int)msg.Type)
								{
									ImGui::TextColored(msg.Color, msg.Text.c_str());
								}
							}
						}
						ImGui::EndChild();
					}
					ImGui::EndChild();

					ImGui::End();
				}
			}
			
		}

		void AddMessage(const std::string& message, LogLevel logLevel)
		{
			Message msg;
			msg.Type = logLevel;

			time_t theTime = time(NULL);
			struct tm* aTime = localtime(&theTime);
			int hour = aTime->tm_hour;
			int min = aTime->tm_min;
			int second = aTime->tm_sec;
			std::ostringstream oss;

			switch (logLevel)
			{
			case LogLevel::Info: msg.Color = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f }; break;
			case LogLevel::Warn: msg.Color = ImVec4{ 2.2f, 1.2f, 0.1f, 1.0f }; break;
			case LogLevel::Error: msg.Color = ImVec4{ 0.99f, 0.1f, 0.1f, 1.0f }; break;
			}

			bool ext_found = message.find("[") != std::string::npos && message.find("]") != std::string::npos;
			oss << "[" << hour << ":" << min << ":" << second << "] ";
			if (ext_found == false) { oss << "[Core]: "; }

			oss << message;
			msg.Text = oss.str();
			m_Messages.push_back(msg);
		}

		void AddMessageInfo(const std::string& message)
		{
			AddMessage(message, LogLevel::Info);
		}

		void AddMessageWarn(const std::string& message)
		{
			AddMessage(message, LogLevel::Warn);
		}

		void AddMessageError(const std::string& message)
		{
			AddMessage(message, LogLevel::Error);
		}

		static EditorConsole* GetConsole() { return s_Instance; }

	private:

		inline static EditorConsole*  s_Instance = nullptr;
		int                           m_CurrentItem = 0;
		std::vector<Message>          m_Messages;
	};
}