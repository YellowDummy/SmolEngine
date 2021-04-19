#pragma once
#include <glm/glm.hpp>
#include <string>

namespace ImGui
{
	class Extensions
	{
	public:

		// Components
		static void TransformComponent(glm::vec3& wordPos, glm::vec3& scale, glm::vec3& rotation);

		// Text
		static void Text(const std::string& label, const std::string& text, float pos = 130.0f, const std::string& additionalID = "None");

		// Float
		static void InputFloat(const std::string& label, float& value, float pos = 130.0f, const std::string& additionalID = "None");
		static void InputFloat2(const std::string& label, glm::vec2& vec2, float resetValue = 0.0f,
			float width = 130.0f, bool enableButtons = true, const std::string& additionalID = "None");
		static void InputFloat3(const std::string& label, glm::vec3& vec3, float resetValue = 0.0f,
			float width = 130.0f, bool enableButtons = true, const std::string& additionalID = "None");

		static void DragFloat2Base(const std::string& label, glm::vec2& vec2, float pos = 130.0f, const std::string& additionalID = "None");
		static void DragFloat3Base(const std::string& label, glm::vec3& vec3, float pos = 130.0f, const std::string& additionalID = "None");

		static void InputFloat2Base(const std::string& label, glm::vec2& vec2, float pos = 130.0f, const std::string& additionalID = "None");
		static void InputFloat3Base(const std::string& label, glm::vec3& vec3, float pos = 130.0f, const std::string& additionalID = "None");

		// Int
		static void InputInt(const std::string& label, int& value, float pos = 130.0f, const std::string& additionalID = "None");

		// String
		static void InputString(const std::string& label, std::string& output, std::string& dummyStr,
			const std::string& hint = "Name", float pos = 130.0f, const std::string& additionalID = "None");
		static bool InputRawString(const std::string& label, std::string& string,
			const std::string& hint = "Name", float pos = 130.0f, bool enterReturnsTrue = true, const std::string& additionalID = "None");

		// Color
		static void ColorInput3(const std::string& label, glm::vec4& color, float pos = 130.0f, const std::string& additionalID = "None");

		// Texture
		static void Texture(const std::string& label, void* textureID, float pos = 130.0f, const std::string& additionalID = "None");

		// Checkbox
		static bool CheckBox(const std::string& label, bool& value, float pos = 130.0f, const std::string& additionalID = "None");

		// Combo
		static void Combo(const std::string& label, const char* comboText, int& value, float pos = 130.0f, const std::string& additionalID = "None");

		// Button
		static bool SmallButton(const std::string& label, const std::string& buttonText, float pos = 130.0f, const std::string& additionalID = "None");
	};
}