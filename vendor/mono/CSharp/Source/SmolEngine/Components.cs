
namespace SmolEngine
{
    public struct HeadComponent
    {
        public readonly string Name;
        public readonly string Tag;
    }

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
