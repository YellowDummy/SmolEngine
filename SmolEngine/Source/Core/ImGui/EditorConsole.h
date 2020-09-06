#pragma once

#include "Core/Core.h"
#include "Core/SLog.h"
#include "../Libraries/imgui/imgui.h"

#include <vector>
#include <ctime>
#include <sstream>

#include "Core/SLog.h"



namespace SmolEngine
{
	enum class LogLevel: int
	{
		None = 0, 
		Info, Error, Warn
	};

	struct Message
	{
		ImVec4 Color = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
		std::string Text = std::string("");
		LogLevel Type = LogLevel::Info;
	};

	class EditorConsole
	{
	public:


		void Update(bool& enabled)
		{
			if (enabled)
			{
				if (ImGui::Begin("Console", &enabled))
				{
					ImGui::BeginChild("ConsoleChild");
					{
						ImGui::SameLine();
						if (ImGui::Button("Clear"))
						{
							m_Messages.clear();
						}

						static int current_item = 0;

						ImGui::SameLine();
						ImGui::Combo("Filter", &current_item, "None\0Info\0Error\0Warn\0\0");

						ImGui::Separator();
						ImGui::BeginChild("Log");
						{
							if (current_item == 0)
							{
								for (auto& msg : m_Messages)
								{
									ImGui::TextColored(msg.Color, msg.Text.c_str());
								}
							}
							else
							{
								for (auto& msg : m_Messages)
								{
									if (current_item == (int)msg.Type)
									{
										ImGui::TextColored(msg.Color, msg.Text.c_str());
									}
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
			case LogLevel::Info:
			{
				oss << "[" << hour << ":" << min << ":" << second << "] " << "[INFO]: " << message;
				msg.Text = oss.str();
				msg.Color = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
				m_Messages.push_back(msg);
				break;
			}
			case LogLevel::Warn:
			{
				oss << "[" << hour << ":" << min << ":" << second << "] " << "[WARN]: " << message;
				msg.Text = oss.str();
				msg.Color = ImVec4{ 2.2f, 1.2f, 0.1f, 1.0f };
				m_Messages.push_back(msg);
				break;
			}
			case LogLevel::Error:
			{
				oss << "[" << hour << ":" << min << ":" << second << "] " << "[ERROR]: " << message;
				msg.Text = oss.str();
				msg.Color = ImVec4{ 0.99f, 0.1f, 0.1f, 1.0f };
				m_Messages.push_back(msg);
				break;
			}

			default:
				NATIVE_WARN("Message wasn't added to the console!");
				break;
			}
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

		static std::shared_ptr<EditorConsole> GetConsole() { return s_EditorConsole; }

	private:
		std::vector<Message> m_Messages;
		static std::shared_ptr<EditorConsole> s_EditorConsole;
	};


#define CONSOLE_INFO(...)  EditorConsole::GetConsole()->AddMessageInfo(__VA_ARGS__)
#define CONSOLE_WARN(...)  EditorConsole::GetConsole()->AddMessageWarn(__VA_ARGS__)
#define CONSOLE_ERROR(...) EditorConsole::GetConsole()->AddMessageError(__VA_ARGS__)
}