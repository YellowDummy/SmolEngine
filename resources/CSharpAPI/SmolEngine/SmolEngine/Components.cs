using System;
using System.Collections.Generic;
using System.Text;

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
    }
}
