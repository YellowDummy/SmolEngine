using System;
using System.Reflection;

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

        public static void OnComponentUpdated<T>(ref T component, uint entity_id) where T : unmanaged
        {
            ushort type = GetComponentType<T>();
            if (type < (ushort)ComponentTypeEX.MaxEnum)
            {
                var obj = component;
                unsafe
                {
                    Actor.SetComponent_EX(&obj, entity_id, type);
                }
            }
        }
    }
}
