#pragma once

#include <glm/glm.hpp>
#include <cereal/cereal.hpp>

namespace physx
{
	class PxRigidActor;
	class PxShape;
	class PxMaterial;
	class PxRigidDynamic;
	class PxRigidStatic;
	class PxTransform;
}

namespace SmolEngine
{
	class Actor;
	struct PhysXWorldSComponent;

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
		float                      HalfHeight = 1.0f;
	};

	struct BoxShapeCreateInfo
	{
		float                      X = 10.0f;
		float                      Y = 10.0f;
		float                      Z = 10.0f;
	};

	struct PhysicsMaterialCreateInfo
	{
		float                      Density = 1.0f;
		float                      StaticFriction = 0.5f;
		float                      DynamicFriction = 0.5f;
		float                      Restitution = 0.6f;
	};

	struct BodyCreateInfo
	{
		Actor*                     pActor = nullptr;
		RigidBodyShape             eShape = RigidBodyShape::Box;
		PhysicsMaterialCreateInfo  Material{};
		BoxShapeCreateInfo         BoxShapeInfo{};
		CapsuleShapeCreateInfo     CapsuleShapeInfo{};
		glm::vec3                  LocalPos = glm::vec3(0, 0, 1);

	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(eShape,
				Material.Density, Material.DynamicFriction, Material.StaticFriction, Material.Restitution,
				BoxShapeInfo.X, BoxShapeInfo.Y, BoxShapeInfo.Z,
				CapsuleShapeInfo.HalfHeight, CapsuleShapeInfo.Radius,
				LocalPos.x, LocalPos.y, LocalPos.z);
		}
	};

	class RigidActor
	{
	public:

		bool IsActive() const;

	private:

		struct ShapeCreateInfo
		{
			physx::PxRigidActor*  Body = nullptr;
			physx::PxShape*       Shape = nullptr;
			physx::PxMaterial*    Material = nullptr;
			BodyCreateInfo*       BodyInfo = nullptr;
		};

		PhysXWorldSComponent*     m_Engine = nullptr;
		physx::PxRigidActor*      m_BaseBody = nullptr;

	private:

		virtual void Create(BodyCreateInfo* info, const glm::vec3& pos, const glm::vec3& rot) = 0;
		virtual void Destroy() = 0;

		void CreateCapsule(ShapeCreateInfo* info);
		void CreateSphere(ShapeCreateInfo* info);
		void CreateBox(ShapeCreateInfo* info);

		void InitRigidActor(BodyCreateInfo* info, physx::PxRigidActor* body);
		void GlmToPxTransform(const glm::vec3& pos, const glm::vec3& rot, physx::PxTransform* transform);

		friend class StaticBody;
		friend class RigidBody;
		friend class PhysicsSystem;
	};
}