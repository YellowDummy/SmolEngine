#pragma once
#include "Core/Core.h"

#include <glm/glm.hpp>
#include <string>

namespace ImGui
{
	class Extensions
	{
	public:

		/// Components

		static void TransformComponent(glm::vec3& wordPos, glm::vec3& scale, glm::vec3& rotation);


		/// Float

		static void InputFloat(const std::string& label, float& value, float pos = 130.0f);


		static void InputFloat2(const std::string& label, glm::vec2& vec2, float resetValue = 0.0f,
			float width = 130.0f, bool enableButtons = true);


		static void InputFloat3(const std::string& label, glm::vec3& vec3, float resetValue = 0.0f,
			float width = 130.0f, bool enableButtons = true);

		static void InputFloat2Base(const std::string& label, glm::vec2& vec2, float pos = 130.0f);

		static void InputFloat3Base(const std::string& label, glm::vec3& vec3, float pos = 130.0f);


		/// Int

		static void InputInt(const std::string& label, int& value, float pos = 130.0f);


		/// String


		static void InputString(const std::string& label, std::string& output, std::string& dummyStr,
			const std::string& hint = "Name", float pos = 130.0f);


		static bool InputRawString(const std::string& label, std::string& string,
			const std::string& hint = "Name", float pos = 130.0f, bool enterReturnsTrue = true);

		/// Color

		static void ColorInput3(const std::string& label, glm::vec4& color, float pos = 130.0f);


		/// Texture


		static void Texture(const std::string& label, uint32_t textureID, float pos = 130.0f);

		/// Checkbox


		static bool CheckBox(const std::string& label, bool& value, float pos = 130.0f);


		/// Combo


		static void Combo(const std::string& label, const char* comboText, int& value, float pos = 130.0f);

		/// Button


		static bool SmallButton(const std::string& label, const std::string& buttonText, float pos = 130.0f);
	};
}