using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SmolEngine
{
    enum ComponentTypeEX: ushort
    {
        TransformComponent,
        CameraComponent,
        RigidBodyComponent,
        RigidBody2DComponent,
        MeshComponent,
        PointLightComponent,
        SpotLightComponent,
        Light2DComponent,
        Texture2DComponent,
        RendererStateComponent,
        CanvasComponent,

        MaxEnum
    }

    class Utils
    {
        static public ushort GetComponentType<T>()
        {
            string[] names = Enum.GetNames(typeof(ComponentTypeEX));
            for(int i = 0;  i < names.Length; i++)
            {
                if (typeof(T).Name == names[i])
                {
                    return (ushort)i;
                }
            }

            return (ushort)ComponentTypeEX.MaxEnum;
        }
    }
}
