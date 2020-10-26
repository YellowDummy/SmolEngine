#include "stdafx.h"
#include "HeadComponent.h"

namespace SmolEngine
{
	HeadComponent::HeadComponent()
	{

	}

	HeadComponent::HeadComponent(const std::string& name, const std::string tag, size_t id)
		:
		Name(name),
		Tag(tag),
		ID(id)
	{

	}
}