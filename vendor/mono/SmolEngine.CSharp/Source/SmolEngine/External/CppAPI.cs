
using System;
using System.Runtime.CompilerServices;

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

    class CppAPI
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        unsafe public extern static bool GetComponent(void* ptr, uint entity_id, ushort component_type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        unsafe public extern static bool SetComponent(void* ptr, uint entity_id, ushort component_type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool HasComponent(uint entity_id, ushort component_type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string GetEntityName(uint entity_id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string GetEntityTag(uint entity_id);
    }
}
