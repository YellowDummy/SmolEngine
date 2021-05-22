#pragma once
#include "Common/Core.h"
#include "Common/Time.h"
#include "Common/Events.h"

#include <glm/glm.hpp>

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	class Camera
	{
	public:

		Camera() = default;
		virtual ~Camera() = default;

		virtual void OnUpdate(DeltaTime deltaTime) = 0;
		virtual void OnEvent(Event& e) = 0;

		virtual const glm::mat4 GetViewProjection() const = 0;
		virtual const glm::mat4& GetProjection() const = 0;
		virtual const glm::mat4& GetViewMatrix() const = 0;
		virtual const glm::vec3& GetPosition() const = 0;

		virtual float GetNearClip() const = 0;
		virtual float GetFarClip() const = 0;
	};
}