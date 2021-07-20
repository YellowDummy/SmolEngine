
using System.Runtime.InteropServices;

namespace SmolEngine
{
    enum ComponentTypeEX
    {
        Null,
        Transform,
        Camera,
        RigidBody,
        RigidBody2D,
        Mesh,
        PointLight,
        SpotLight,
        Light2D,
        Texture2D,
        RendererState,
        Canvas
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct TransformComponent
    {
        public Vector3 WorldPos;
        public Vector3 Rotation;
        public Vector3 Scale;

        public TransformComponent(Vector3 pos, Vector3 rot, Vector3 scale)
        {
            WorldPos = pos;
            Rotation = rot;
            Scale = scale;
        }

        public override string ToString()
        {
            return "Pos:" + WorldPos.ToString() + " Rot:" + Rotation.ToString() + " Scale:" + Scale.ToString();
        }
    }
}
