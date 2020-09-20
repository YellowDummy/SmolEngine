#pragma once
#include "Core/Core.h"

#include <cereal/cereal.hpp>

namespace SmolEngine
{
	class AudioClip
	{
	public:
		AudioClip() = default;

	private:
		friend class cereal::access;
	};
}