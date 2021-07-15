
using System.Runtime.InteropServices;

namespace SmolEngine
{
    [StructLayout(LayoutKind.Sequential)]
    public struct HeadComponent
    {
        public string Name;
        public string Tag;                                                                                                                                                                                                                                                      
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct TransformComponent
    {
        public Vector3 WorldPos;
        public Vector3 Rotation;
        public Vector3 Scale;

        public override string ToString()
        {
            return "Pos:" + WorldPos.ToString() + " Rot:" + Rotation.ToString() + " Scale:" + Scale.ToString();
        }
    }
}
