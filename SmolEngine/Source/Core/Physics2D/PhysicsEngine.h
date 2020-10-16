#pragma once

#include "Core/Core.h"
#include "Core/Time.h"
#include <glm/glm.hpp>

namespace SmolEngine
{
	enum class EngineType : uint16_t
	{
		Box2D, Bullet
	};

	class PhysicsEngine
	{
	public:

		PhysicsEngine() = default;

		/// Overrides

		virtual void Init(const glm::vec2& gravity = glm::vec2(0.0f, -8.91f)) {}

		virtual void OnPlay() {}

		virtual void OnEndPlay() {}

		virtual void Reset() {}

		virtual void Update(DeltaTime delta, uint32_t velocityIterations, uint32_t positionIterations) {}

		/// Getters

		virtual void* GetWorld() { return nullptr; }

		/// Init

		static std::unique_ptr<PhysicsEngine> Create(EngineType type);
	};
}