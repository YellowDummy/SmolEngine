using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace SmolEngine
{
    public enum RigidBodyShape: ushort
    {
        Box,
        Sphere,
        Capsule,
        Custom
    }

    public enum RigidBodyType: ushort
    {
        Dynamic,
        Static,
        Kinematic
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct RigidBodyCreateInfo
    {
        public RigidBodyShape eShape;
        public RigidBodyType eType;
        public float Mass;
        public float Density;
        public float Friction;
        public float Restitution;
        public float LinearDamping;
        public float AngularDamping;
        public float RollingFriction;
        public float SpinningFriction;
        public Vector3 Size;
        public string Path; // fo custom geometry

        public void CreateDefault()
        {
            Size = new Vector3(1, 1, 1);
            eShape = RigidBodyShape.Box;
            eType = RigidBodyType.Dynamic;
            Mass = 1.0f;
            Density = 0.5f;
            Friction = 0.5f;
            Restitution = 0.0f;
            LinearDamping = 0.0f;
            AngularDamping = 0.0f;
            RollingFriction = 0.1f;
            SpinningFriction = 0.1f;
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct RigidBodyComponent
    {
        private enum ImpactFlags: ushort
        {
            Force,
            Impulse,
            Torque,
            Gravity
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void CreateRigidBody_EX(ref RigidBodyCreateInfo info, uint entity_id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void SetImpact_EX(ref Vector3 value, uint entity_id, ImpactFlags flags);

        private readonly uint _Handler;
        private RigidBodyShape _Shape;
        private RigidBodyType _Type;
        private bool _IsCreated;

        public void Create(RigidBodyCreateInfo info)
        {
            _Shape = info.eShape;
            _Type = info.eType;
            _IsCreated = true;

            CreateRigidBody_EX(ref info, _Handler);
        }

        public bool IsActive()
        {
            return _IsCreated;
        }

        public RigidBodyType GetBodyType()
        {
            return _Type;
        }

        public RigidBodyShape GetBodyShape()
        {
            return _Shape;
        }

        public void AddForce(Vector3 force)
        {
            SetImpact_EX(ref force, _Handler, ImpactFlags.Force);
        }

        public void AddImpulse(Vector3 impulse)
        {
            SetImpact_EX(ref impulse, _Handler, ImpactFlags.Impulse);
        }

        public void AddTorque(Vector3 torque)
        {
            SetImpact_EX(ref torque, _Handler, ImpactFlags.Torque);
        }

        public void SetGravity(Vector3 gravity)
        {
            SetImpact_EX(ref gravity, _Handler, ImpactFlags.Gravity);
        }
    }
}
