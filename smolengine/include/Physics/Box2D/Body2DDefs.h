#pragma once

#include <stdafx.h>
#include <glm/glm.hpp>

namespace SmolEngine
{
	enum class Body2DType : uint16_t
	{
		Static,
		Kinematic,
		Dynamic
	};

	enum class Shape2DType : uint16_t
	{
		Box,
		Cirlce
	};

	enum class JointType : uint16_t
	{
		Distance,
		Revolute,
		Prismatic, 
		Pulley,
		Gear,
		Mouse, 
		Wheel,
		Weld,
		Rope,
		Friction,
		Motor
	};

	struct JointInfo
	{
		JointInfo() = default;

		glm::vec2 LocalAnchorA = { 0.0f, 0.0f };
		glm::vec2 LocalAnchorB = { 0.0f, 0.0f };
		bool      CollideConnected = true;
	};

	struct DistanceJointInfo : public JointInfo
	{
		float Length = 1.0f;
		float Stiffness = 0.0f;
		float Damping = 0.0f;
	};

	struct RevoluteJointInfo: public JointInfo
	{
		float ReferenceAngle = 0.0f;
		float LowerAngle = 0.0f;
		float UpperAngle = 0.0f;
		float MaxMotorTorque = 0.0f;
		float MotorSpeed = 0.0f;
		bool EnableLimit = false;
		bool EnableMotor = false;
	};

	struct PrismaticJointInfo : public JointInfo
	{
		glm::vec2 LocalAxisA = { 1.0f, 0.0f };
		float ReferenceAngle = 0.0f;
		float LowerTranslation = 0.0f;
		float UpperTranslation = 0.0f;
		float MaxMotorForce = 0.0f;
		float MotorSpeed = 0.0f;
		bool EnableLimit = false;
		bool EnableMotor = false;
	};

	struct RopeJointInfo : public JointInfo
	{
		float MaxLength = 0.0f;
	};

 }