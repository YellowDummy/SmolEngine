#include "stdafx.h"
#include "PhysicsEngine.h"
#include "Core/SLog.h"

#include "Core/Physics2D/Box2D/Box2DEngine.h"

namespace SmolEngine
{
	std::unique_ptr<PhysicsEngine> PhysicsEngine::Create(EngineType type)
	{
		switch (type)
		{
		case SmolEngine::EngineType::Box2D:
		{
			return std::make_unique<Box2DEngine>();
		}
		case SmolEngine::EngineType::Bullet:
		{
			NATIVE_ERROR("Bullet Engine is not supported!");
			return nullptr;
		}
		default:
			return nullptr;
		}
	}
}