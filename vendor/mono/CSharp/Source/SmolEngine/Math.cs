
namespace SmolEngine
{
    public struct Vector2
    {
        public float X;
        public float Y;
    }

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
