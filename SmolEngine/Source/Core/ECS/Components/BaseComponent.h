#pragma once
#include "Core/Core.h"

namespace SmolEngine
{
	class BaseComponent
	{
	public:

		BaseComponent() = default;

		BaseComponent(uint32_t id)
			:ComponentID(id) {}

	public:

		uint32_t ComponentID = 0;
	};
}