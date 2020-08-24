#pragma once

#include "Core/Core.h"
#include "Core/SLog.h"
#include "../Libraries/imgui/imgui.h"

#include <vector>
#include <functional>

namespace SmolEngine
{
	enum class LogLevel: uint16_t
	{
		Info, Warn, Error
	};

	struct Message
	{
		ImVec4 Color;
		std::string Text;
	};

	class EditorConsole
	{
	public:

		void Update()
		{
			if (ImGui::Begin("Console"))
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
				if (ImGui::BeginChild("Log"))
				{

					for (auto& msg : m_Messages)
					{
						ImGui::TextColored(msg.Color, msg.Text.c_str());
					}

					ImGui::EndChild();
				}

				ImGui::End();
			}
		}

		void AddMessage(std::string& message, LogLevel logLevel)
		{
			Message msg;

			switch (logLevel)
			{
			case LogLevel::Info:
			{
				msg.Text = std::string("[INFO]: ") + message;
				msg.Color = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
				m_Messages.push_back(msg);
				break;
			}
			case LogLevel::Warn:
			{
				msg.Text = std::string("[Warn]: ") + message;
				msg.Color = ImVec4{ 2.2f, 1.2f, 0.1f, 1.0f };
				m_Messages.push_back(msg);
				break;
			}
			case LogLevel::Error:
			{
				msg.Text = std::string("[Error]: ") + message;
				msg.Color = ImVec4{ 0.99f, 0.1f, 0.1f, 1.0f };
				m_Messages.push_back(msg);
				break;
			}

			default:
				NATIVE_WARN("Message wasn't added to the console!");
				break;
			}
		}

	private:
		std::vector<Message> m_Messages;
	};
}