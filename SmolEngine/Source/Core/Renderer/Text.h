#pragma once

#include "Core/Core.h"

#include <string>
#include <glm/glm.hpp>

namespace SmolEngine
{
	class Text
	{
	public:

		Text() = default;

		virtual ~Text() = default;

		//Setters

		virtual void SetText(const std::string& text) {}

		virtual void SetFont(const std::string& fontFilePath) {};

		//Rendering

		virtual void Render(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color, float padding) {}

		//Factory

		//ttf-format
		static Ref<Text> Create(const std::string& fontFilePath);
	};
}