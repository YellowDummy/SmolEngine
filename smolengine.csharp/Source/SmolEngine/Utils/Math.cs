
using System.Runtime.InteropServices;

namespace SmolEngine
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector2
    {
        public float X;
        public float Y;

        public Vector2(float _x, float _y)
        {
            X = _x;
            Y = _y;
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct Vector3
    {
        public float X;
        public float Y;
        public float Z;

        public Vector3(float _x, float _y, float _z)
        {
            X = _x;
            Y = _y;
            Z = _z;
        }

        public override string ToString()
        {
            return "X:" + X + " Y:" + Y + " Z:" + Z;
        }
    }
}
