#pragma once

#include <glm/glm.hpp>
#include <cereal/cereal.hpp>

class btCollisionShape;
class btTransform;
class btRigidBody;

namespace SmolEngine
{
	class Actor;
	class Mesh;
	struct Bullet3WorldSComponent;

	enum class RigidBodyType: int
	{
		Dynamic,
		Static,
		Kinematic
	};

	enum class RigidBodyShape: int
	{
		Box,
		Sphere,
		Capsule,
		Convex
	};

	struct BodyCreateInfo
	{
		Ref<Actor>                 pActor = nullptr;
		Ref<Mesh>                  Mesh = nullptr;
		int                        ShapeIndex = 0;
		int                        StateIndex = 1;
		float                      Mass = 1.0f;
		float                      Density = 0.5f;
		float                      Friction = 0.5f;
		float                      Restitution = 0.0f;
		float                      LinearDamping = 0.0f;
		float                      AngularDamping = 0.0f;
		float                      RollingFriction = 0.1f;
		float                      SpinningFriction = 0.1f;
		RigidBodyShape             eShape = RigidBodyShape::Box;
		RigidBodyType              eType = RigidBodyType::Static;
		std::string                FilePath = "";
		glm::vec3                  Size = glm::vec3(1.0);
		glm::vec3                  LocalInertia = glm::vec3(1, 0, 0);

	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(pActor, eShape, eType, StateIndex, ShapeIndex, Mass, Density,
				Friction, Restitution, LinearDamping, AngularDamping,
				RollingFriction, SpinningFriction,
				Size.x, Size.y, Size.z, FilePath,
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
		void                     CreateConvex(BodyCreateInfo* info);
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