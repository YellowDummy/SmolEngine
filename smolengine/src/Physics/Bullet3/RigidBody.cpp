#include "stdafx.h"
#include "Physics/Bullet3/RigidBody.h"

#include <btBulletDynamicsCommon.h>

namespace SmolEngine
{
    void RigidBody::Create(BodyCreateInfo* info, const glm::vec3& pos, const glm::vec3& rot)
    {
        btTransform transform;
        GLMToBulletTransform(pos, rot, &transform);
        InitBase(info);

        btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
        btVector3 localInertia(info->LocalInertia.x, info->LocalInertia.y, info->LocalInertia.z);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(info->Mass, myMotionState, m_Shape, localInertia);
        rbInfo.m_friction = info->Friction;
        rbInfo.m_restitution = info->Restitution;
        rbInfo.m_linearDamping = info->LinearDamping;
        rbInfo.m_angularDamping = info->AngularDamping;
        rbInfo.m_rollingFriction = info->RollingFriction;
        rbInfo.m_spinningFriction = info->SpinningFriction;
        m_Body = new btRigidBody(rbInfo);

        if (info->eType == RigidBodyType::Kinematic)
        {
            m_Body->setCollisionFlags(m_Body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            m_Body->setActivationState(DISABLE_DEACTIVATION);
        }

        m_Body->setUserPointer(info->pActor);
        SetActive(true);
    }
}