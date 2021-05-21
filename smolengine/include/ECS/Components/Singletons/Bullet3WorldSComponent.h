#pragma once

#include "Core/Core.h"
#include <glm/glm.hpp>

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

namespace SmolEngine
{
	class PhysXAllocator;
	class PhysXErrorCallback;

	struct PhysicsContextCreateInfo
	{
		float      Speed = 981;
		uint32_t   NumWorkThreads = 2;
		glm::vec3  Gravity = { 0.0f, -9.81f, 0.0f };
	};

	// Note:
    // S - Singleton Component

	struct Bullet3WorldSComponent
	{
		Bullet3WorldSComponent();
		Bullet3WorldSComponent(PhysicsContextCreateInfo* info);
		~Bullet3WorldSComponent();

		void Init(PhysicsContextCreateInfo* info);
		static Bullet3WorldSComponent* Get() { return Instance; }

	public:

		btDefaultCollisionConfiguration*       Config = nullptr;
		btCollisionDispatcher*                 Dispatcher = nullptr;
		btBroadphaseInterface*                 Broadphase = nullptr;
		btSequentialImpulseConstraintSolver*   Solver = nullptr;
		btDiscreteDynamicsWorld*               World = nullptr;

		PhysicsContextCreateInfo               CreateInfo{};
	private:

		static Bullet3WorldSComponent* Instance;
	};
}