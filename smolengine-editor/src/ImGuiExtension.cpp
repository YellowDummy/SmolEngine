#include "stdafx.h"
#include "ImGUI/ImGuiExtension.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

namespace ImGui
{
	void Extensions::TransformComponent(glm::vec3& wordPos, glm::vec3& scale, glm::vec3& rotation)
	{
		InputFloat3("Translation", wordPos);
		InputFloat3("Rotation", rotation);
		InputFloat3("Scale", scale, 1.0f);
	}

	void Extensions::InputFloat3(const std::string& label, glm::vec3& vec3, float resetValue, float width, bool enableButtons, const std::string& additionalID)
	{
		ImGui::PushID(std::string(label + additionalID).c_str());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 6, 18 });

		ImGui::Columns(2, label.c_str());
		{
			ImGui::SetColumnWidth(0, width);
			ImGui::TextUnformatted(label.c_str());
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

			float height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 buttonSize = { height + 3.0f, height };

			// X

			{
				if (enableButtons)
				{
					// Color

					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });

					if (ImGui::Button("X", buttonSize))
					{
						vec3.x = resetValue;
					}

					ImGui::PopStyleColor(3);

					///

					ImGui::SameLine();
				}

				ImGui::InputFloat("##X", &vec3.x);
				ImGui::PopItemWidth();
				ImGui::SameLine();
			}

			// Y

			{
				if (enableButtons)
				{
					// Color

					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });

					if (ImGui::Button("Y", buttonSize))
					{
						vec3.y = resetValue;
					}

					ImGui::PopStyleColor(3);

					ImGui::SameLine();
				}

				ImGui::InputFloat("##Y", &vec3.y);
				ImGui::PopItemWidth();
				ImGui::SameLine();
			}

			// Z

			{
				if (enableButtons)
				{
					// Color

					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });

					if (ImGui::Button("Z", buttonSize))
					{
						vec3.z = resetValue;
					}

					ImGui::PopStyleColor(3);

					ImGui::SameLine();
				}

				ImGui::InputFloat("##Z", &vec3.z);
				ImGui::PopItemWidth();
				ImGui::SameLine();
			}

		}

		ImGui::Columns(1);
		ImGui::PopStyleVar();
		ImGui::PopID();
	}

	void Extensions::DragFloat2Base(const std::string& label, glm::vec2& vec2, float pos, const std::string& additionalID)
	{
		ImGui::PushID(std::string(label + additionalID).c_str());

		ImGui::SetCursorPosX(6);
		ImGui::TextUnformatted(label.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(pos);
		ImGui::DragFloat2("##F", glm::value_ptr(vec2));

		ImGui::PopID();
	}

	void Extensions::DragFloat3Base(const std::string& label, glm::vec3& vec3, float pos, const std::string& additionalID)
	{
		ImGui::PushID(std::string(label + additionalID).c_str());

		ImGui::SetCursorPosX(6);
		ImGui::TextUnformatted(label.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(pos);
		ImGui::DragFloat3("##F", glm::value_ptr(vec3));

		ImGui::PopID();
	}

	void Extensions::InputFloat2Base(const std::string& label, glm::vec2& vec2, float pos, const std::string& additionalID)
	{
		ImGui::PushID(std::string(label + additionalID).c_str());

		ImGui::SetCursorPosX(6);
		ImGui::TextUnformatted(label.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(pos);
		ImGui::InputFloat2("##F", glm::value_ptr(vec2));

		ImGui::PopID();
	}

	void Extensions::InputFloat3Base(const std::string& label, glm::vec3& vec3, float pos, const std::string& additionalID)
	{
		ImGui::PushID(label.c_str());

		ImGui::SetCursorPosX(6);
		ImGui::TextUnformatted(label.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(pos);
		ImGui::InputFloat3("##F", glm::value_ptr(vec3));

		ImGui::PopID();
	}

	void Extensions::InputInt(const std::string& label, int& value, float pos, const std::string& additionalID)
	{
		ImGui::PushID(std::string(label + additionalID).c_str());

		ImGui::SetCursorPosX(6);
		ImGui::TextUnformatted(label.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(pos);
		ImGui::InputInt("##I", &value);

		ImGui::PopID();
	}

	void Extensions::Text(const std::string& label, const std::string& text, float pos, const std::string& additionalID)
	{
		ImGui::PushID(std::string(label + additionalID).c_str());

		ImGui::SetCursorPosX(6);
		ImGui::TextUnformatted(label.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(pos);
		ImGui::TextUnformatted(text.c_str());

		ImGui::PopID();
	}

	bool Extensions::InputFloat(const std::string& label, float& value, float pos, const std::string& additionalID, float text_pos)
	{
		bool use = false;
		ImGui::PushID(std::string(label + additionalID).c_str());

		ImGui::SetCursorPosX(text_pos);
		ImGui::TextUnformatted(label.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(pos);
		use = ImGui::InputFloat("##F", &value);

		ImGui::PopID();
		return use;
	}

	void Extensions::InputFloat2(const std::string& label, glm::vec2& vec2, float resetValue, float width, bool enableButtons, const std::string& additionalID)
	{
		ImGui::PushID(std::string(label + additionalID).c_str());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 6, 25 });
		ImGui::Columns(2);
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPos().y + 10);
			ImGui::SetColumnWidth(0, width);
			ImGui::TextUnformatted(label.c_str());
			ImGui::NextColumn();

			ImGui::SetCursorPosY(ImGui::GetCursorPos().y + 10);
			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

			float height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 buttonSize = { height + 3.0f, height };

			// X

			{
				if (enableButtons)
				{
					// Color

					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });

					if (ImGui::Button("X", buttonSize))
					{
						vec2.x = resetValue;
					}

					ImGui::PopStyleColor(3);

					///

					ImGui::SameLine();
				}

				ImGui::InputFloat("##X", &vec2.x);
				ImGui::PopItemWidth();
				ImGui::SameLine();
			}

			// Y

			{
				if (enableButtons)
				{
					// Color

					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });

					if (ImGui::Button("Y", buttonSize))
					{
						vec2.y = resetValue;
					}

					ImGui::PopStyleColor(3);

					ImGui::SameLine();
				}

				ImGui::InputFloat("##Y", &vec2.y);
				ImGui::PopItemWidth();
				ImGui::SameLine();
			}
		}

		ImGui::Columns(1);
		ImGui::PopStyleVar();
		ImGui::PopID();
	}

	void Extensions::InputString(const std::string& label, std::string& output, std::string& dummyStr, const std::string& hint, float pos, const std::string& additionalID)
	{
		ImGui::PushID(std::string(label + additionalID).c_str());

		ImGui::SetCursorPosX(6);
		ImGui::TextUnformatted(label.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(pos);


		if (ImGui::InputTextWithHint("##N", hint.c_str(), &dummyStr, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			output = dummyStr;
		}

		ImGui::PopID();
	}

	bool Extensions::InputRawString(const std::string& label, std::string& string, const std::string& hint, float pos, bool enterReturnsTrue, const std::string& additionalID)
	{
		ImGui::SetCursorPosX(6);
		ImGui::TextUnformatted(label.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(pos);
		if (enterReturnsTrue)
		{
			return ImGui::InputTextWithHint("##N", hint.c_str(), &string, ImGuiInputTextFlags_EnterReturnsTrue);
		}

		return ImGui::InputTextWithHint("##N", hint.c_str(), &string);
	}

	void Extensions::ColorInput3(const std::string& label, glm::vec4& color, float pos, const std::string& additionalID)
	{
		ImGui::PushID(std::string(label + additionalID).c_str());

		ImGui::SetCursorPosX(6);
		ImGui::TextUnformatted(label.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(pos);
		ImGui::ColorEdit3("##C", glm::value_ptr(color));

		ImGui::PopID();
	}

	void Extensions::Texture(const std::string& label, void* textureID, float pos, const std::string& additionalID)
	{
		ImGui::PushID(std::string(label + additionalID).c_str());

		ImGui::SetCursorPosX(6);
		ImGui::TextUnformatted(label.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(pos);
		ImGui::Image(textureID, ImVec2{ 100, 100 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::PopID();
	}

	bool Extensions::CheckBox(const std::string& label, bool& value, float pos,const std::string& additionalID, float text_pos)
	{
		ImGui::PushID(std::string(label + additionalID).c_str());

		ImGui::SetCursorPosX(text_pos);
		ImGui::TextUnformatted(label.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(pos);
		bool result = ImGui::Checkbox("##C", &value);
		ImGui::PopID();

		return result;
	}

	bool Extensions::Combo(const std::string& label, const char* comboText, int& value, float pos, const std::string& additionalID, float text_pos)
	{
		ImGui::PushID(std::string(label + additionalID).c_str());

		bool use = false;
		ImGui::SetCursorPosX(text_pos);
		ImGui::TextUnformatted(label.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(pos);
		use = ImGui::Combo("##C", &value, comboText);

		ImGui::PopID();
		return use;
	}

	bool Extensions::SmallButton(const std::string& label, const std::string& buttonText, float pos, const std::string& additionalID)
	{
		ImGui::SetCursorPosX(6);
		ImGui::TextUnformatted(label.c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(pos);
		return ImGui::SmallButton(buttonText.c_str());
	}

}