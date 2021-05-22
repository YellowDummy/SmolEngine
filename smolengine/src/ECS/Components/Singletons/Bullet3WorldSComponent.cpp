#include "stdafx.h"
#include "ECS/Components/Singletons/Bullet3WorldSComponent.h"

#include <btBulletDynamicsCommon.h>
#include <LinearMath/btIDebugDraw.h>
#include <Frostium3D/DebugRenderer.h>

namespace SmolEngine
{
	class BulletDebugDraw: public btIDebugDraw
	{
	public:

		void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override
		{
			DebugRenderer::DrawLine({ from.x(), from.y(), from.z() }, { to.x(), to.y(), to.z() });
		}

		void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override
		{
			btVector3 to = PointOnB + normalOnB * distance;
			const btVector3& from = PointOnB;
			DebugRenderer::DrawLine({ from.x(), from.y(), from.z() }, { to.x(), to.y(), to.z() });
		}

		void reportErrorWarning(const char* warningString) override
		{
			NATIVE_ERROR(warningString); 
		}

		void draw3dText(const btVector3& location, const char* textString) override {}
		void setDebugMode(int debugMode) override { m_debugMode = debugMode; }
		int getDebugMode() const override { return m_debugMode; }

	private:

		int m_debugMode = 0;
	};

	Bullet3WorldSComponent* Bullet3WorldSComponent::Instance = nullptr;

	Bullet3WorldSComponent::Bullet3WorldSComponent()
	{
		PhysicsContextCreateInfo info{};
		Init(&info);
	}

	Bullet3WorldSComponent::Bullet3WorldSComponent(PhysicsContextCreateInfo* info)
	{
		Init(info);
	}

	Bullet3WorldSComponent::~Bullet3WorldSComponent()
	{
	}

	void Bullet3WorldSComponent::Init(PhysicsContextCreateInfo* info)
	{
		Config = new btDefaultCollisionConfiguration();
		Dispatcher = new btCollisionDispatcher(Config);
		Broadphase = new btDbvtBroadphase();
		Solver = new btSequentialImpulseConstraintSolver;

		World = new btDiscreteDynamicsWorld(Dispatcher, Broadphase, Solver, Config);
		World->setGravity(btVector3(info->Gravity.x, info->Gravity.y, info->Gravity.z));

#ifdef SMOLENGINE_EDITOR
		DebugDraw = new BulletDebugDraw();
		World->setDebugDrawer(DebugDraw);
		DebugDraw->setDebugMode(1);
#endif

		Instance = this;
	}
}