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
		float                      Mass = 1.0f;
		float                      Density = 0.5f;
		RigidBodyShape             eShape = RigidBodyShape::Box;
		BoxShapeCreateInfo         BoxShapeInfo{};
		SphereShapeCreateInfo      SphereShape{};
		CapsuleShapeCreateInfo     CapsuleShapeInfo{};
		glm::vec3                  LocalPos = glm::vec3(0, 0, 1);

	private:

		friend class cereal::access;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(eShape, ShapeIndex, ActorID, Mass, Density,
				SphereShape.Radius,
				BoxShapeInfo.X, BoxShapeInfo.Y, BoxShapeInfo.Z,
				CapsuleShapeInfo.Height, CapsuleShapeInfo.Radius,
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