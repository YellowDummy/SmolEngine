#pragma once
#include "Core/Core.h"
#include <glm/glm.hpp>

namespace SmolEngine
{
	enum class UIElementType: uint16_t
	{
		None = 0,
		TextLabel,
		Button,
		TextBox
	};

	class UIElement
	{
	public:

		UIElement() =  default;

		virtual ~UIElement() = default;

		virtual void Reload() = 0;

	public:

		size_t m_ID = 0;
		UIElementType m_Type = UIElementType::None;
	};
}