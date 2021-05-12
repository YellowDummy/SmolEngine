#pragma once

#include "Core/Core.h"
#include <glm/glm.hpp>

namespace physx
{
	class PxFoundation;
	class PxPhysics;
	class PxScene;
	class PxMaterial;
}

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

	struct PhysXWorldSComponent
	{
		PhysXWorldSComponent();
		PhysXWorldSComponent(PhysicsContextCreateInfo* info);
		~PhysXWorldSComponent();

		void Init(PhysicsContextCreateInfo* info);
		static PhysXWorldSComponent* Get() { return Instance; }

	public:

		PhysicsContextCreateInfo  CreateInfo{};
		physx::PxFoundation*      mFoundation = nullptr;
		physx::PxPhysics*         mPhysics = nullptr;
		physx::PxScene*           mScene = nullptr;
		physx::PxMaterial*        mDefaultMaterial = nullptr;
		Ref<PhysXAllocator>       mAllocator = nullptr;
		Ref<PhysXErrorCallback>   mErrorCallback = nullptr;

	private:

		static PhysXWorldSComponent* Instance;
	};
}