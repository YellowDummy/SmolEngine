#pragma once

#include <glm/glm.hpp>
#include <cereal/cereal.hpp>

class btCollisionShape;
class btTransform;
class btRigidBody;

namespace SmolEngine
{
	class Actor;
	struct Bullet3WorldSComponent;

	enum class RigidBodyShape: int
	{
		Sphere,
		Capsule,
		Box,
		Custom
	};

	struct CapsuleShapeCreateInfo
	{
		float                      Radius = 1.0f;
		float                      Height = 11.0f;
	};

	struct BoxShapeCreateInfo
	{
		float                      X = 1.0f;
		float                      Y = 1.0f;
		float                      Z = 1.0f;
	};

	struct SphereShapeCreateInfo
	{
		float                      Radius = 1.0f;
	};

	struct BodyCreateInfo
	{
		Actor*                     pActor = nullptr;
		uint32_t                   ActorID = 0;
		int                        ShapeIndex = 0;
		int                        StateIndex = 0;
		float                      Mass = 1.0f;
		float                      Density = 0.5f;
		float                      Friction = 0.5f;
		float                      Restitution = 0.0f;
		float                      LinearDamping = 0.0f;
		float                      AngularDamping = 0.0f;
		float                      RollingFriction = 0.1f;
		float                      SpinningFriction = 0.1f;
		RigidBodyShape             eShape = RigidBodyShape::Box;
		BoxShapeCreateInfo         BoxShapeInfo{};
		SphereShapeCreateInfo      SphereShape{};
		CapsuleShapeCreateInfo     CapsuleShapeInfo{};
		glm::vec3                  LocalInertia = glm::vec3(1, 0, 0);

	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(eShape, StateIndex, ShapeIndex, ActorID, Mass, Density,
				Friction, Restitution, LinearDamping, AngularDamping,
				RollingFriction, SpinningFriction,
				SphereShape.Radius,
				BoxShapeInfo.X, BoxShapeInfo.Y, BoxShapeInfo.Z,
				CapsuleShapeInfo.Height, CapsuleShapeInfo.Radius,
				LocalInertia.x, LocalInertia.y, LocalInertia.z);
		}
	};

	class RigidActor
	{
	public:

		bool                     IsActive() const;
		static void              GLMToBulletTransform(const glm::vec3& pos, const glm::vec3& rot, btTransform* transform);
		static void              BulletToGLMTransform(const btTransform* transform, glm::vec3& pos, glm::vec3& rot);

	private:

		void                     InitBase(BodyCreateInfo* info);
		virtual void             Create(BodyCreateInfo* info, const glm::vec3& pos, const glm::vec3& rot) = 0;
		void                     CreateCapsule(BodyCreateInfo* info);
		void                     CreateSphere(BodyCreateInfo* info);
		void                     CreateBox(BodyCreateInfo* info);
		void                     SetActive(bool value);

	private:

		btCollisionShape*        m_Shape = nullptr;
		btRigidBody*             m_Body = nullptr;
		bool                     m_Active = false;

	private:

		friend class StaticBody;
		friend class RigidBody;
		friend class PhysicsSystem;
	};
}