#include "stdafx.h"
#include "Physics/Bullet3/RigidActor.h"
#include "ECS/Components/Singletons/Bullet3WorldSComponent.h"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>

#include <Frostium3D/Utils/Utils.h>
#include <Frostium3D/Utils/glTFImporter.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace SmolEngine
{
	void RigidActor::GLMToBulletTransform(const glm::vec3& pos, const glm::vec3& rot, btTransform* transform)
	{
		glm::mat4 model;
		Utils::ComposeTransform(pos, rot, { 1,1, 1 }, model);

		transform->setIdentity();
		transform->setFromOpenGLMatrix(glm::value_ptr(model));
	}

	void RigidActor::BulletToGLMTransform(const btTransform* transform, glm::vec3& pos, glm::vec3& rot)
	{
		const auto& origin = transform->getOrigin();
		float x, y, z;
		{
			const auto& rotation = transform->getRotation();
			const glm::mat4 m = glm::toMat4(glm::quat(rotation.getW(), rotation.getX(), rotation.getY(), rotation.getZ()));
			glm::extractEulerAngleXYZ(m, x, y, z);
		}

		pos.x = origin.x();
		pos.y = origin.y();
		pos.z = origin.z();
		rot = glm::vec3(x, y, z);
	}

	bool RigidActor::IsActive() const
	{
		return m_Active;
	}

	void RigidActor::InitBase(BodyCreateInfo* info)
	{
		switch (info->eShape)
		{
		case RigidBodyShape::Box:     CreateBox(info); break;
		case RigidBodyShape::Sphere:  CreateSphere(info); break;
		case RigidBodyShape::Capsule: CreateCapsule(info); break;
		case RigidBodyShape::Convex:  CreateConvex(info); break;
		}

		if (info->StateIndex == 1) // static
			info->Mass = 0.0f;

		bool isDynamic = info->Mass != 0.0f;
		btVector3 inertia{};
		if(isDynamic)
			m_Shape->calculateLocalInertia(info->Mass, inertia);

		info->LocalInertia.x = inertia.x();
		info->LocalInertia.y = inertia.y();
		info->LocalInertia.z = inertia.z();
	}

	void RigidActor::CreateCapsule(BodyCreateInfo* info)
	{
		m_Shape = new btCapsuleShape(info->CapsuleShapeInfo.Radius, info->CapsuleShapeInfo.Height);
	}

	void RigidActor::CreateSphere(BodyCreateInfo* info)
	{
		m_Shape = new btSphereShape(info->SphereShape.Radius);
	}

	void RigidActor::CreateBox(BodyCreateInfo* info)
	{
		m_Shape = new btBoxShape(btVector3(info->BoxShapeInfo.X, info->BoxShapeInfo.Y, info->BoxShapeInfo.Z));
	}

	void RigidActor::CreateConvex(BodyCreateInfo* info)
	{
		if (info->ConvexShapeInfo.FilePath.empty() == false)
		{
			ImportedDataGlTF* data = new ImportedDataGlTF();
			if (glTFImporter::Import(info->ConvexShapeInfo.FilePath, data))
			{
				auto& model = data->Primitives[0];
				auto trimesh = new btTriangleMesh();
				for (uint32_t i = 0; i < static_cast<uint32_t>(model.IndexBuffer.size()); i+=3)
				{
					uint32_t index0 = model.IndexBuffer[i];
					uint32_t index1 = model.IndexBuffer[i + 1];
					uint32_t index2 = model.IndexBuffer[i + 2];

					btVector3 vertex0(model.VertexBuffer[index0].Pos.x, model.VertexBuffer[index0].Pos.y, model.VertexBuffer[index0].Pos.z);
					btVector3 vertex1(model.VertexBuffer[index1].Pos.x, model.VertexBuffer[index1].Pos.y, model.VertexBuffer[index1].Pos.z);
					btVector3 vertex2(model.VertexBuffer[index2].Pos.x, model.VertexBuffer[index2].Pos.y, model.VertexBuffer[index2].Pos.z);

					trimesh->addTriangle(vertex0, vertex1, vertex2);
				}

				auto trimeshShape = new btBvhTriangleMeshShape(trimesh, true);
				m_Shape = trimeshShape;
			}

			delete data;
		}
	}

	void RigidActor::SetActive(bool value)
	{
		m_Active = value;
	}
}