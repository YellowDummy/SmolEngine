using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SmolEngine
{
    enum ComponentTypeEX
    {
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
        Canvas,
        MaxEnum
    }

    class Utils
    {
        static public ushort GetComponentType<T>()
        {
            if (typeof(T) == typeof(TransformComponent))
            {
                return (ushort)ComponentTypeEX.Transform;
            }

            return (ushort)ComponentTypeEX.MaxEnum;
        }
    }
}
