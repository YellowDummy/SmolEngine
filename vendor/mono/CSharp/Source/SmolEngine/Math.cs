
using System.Runtime.InteropServices;

namespace SmolEngine
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector2
    {
        public float X;
        public float Y;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct Vector3
    {
        public float X;
        public float Y;
        public float Z;

        public override string ToString()
        {
            return "X:" + X + " Y:" + Y + " Z:" + Z;
        }
    }
}
