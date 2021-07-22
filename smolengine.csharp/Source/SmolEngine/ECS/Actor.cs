using System;
using System.Runtime.CompilerServices;

namespace SmolEngine
{
    public class Actor
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal unsafe extern static bool GetComponent_EX(void* ptr, uint entity_id, ushort component_type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal unsafe extern static bool SetComponent_EX(void* ptr, uint entity_id, ushort component_type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal unsafe extern static bool AddComponent_EX(void* ptr, uint entity_id, ushort component_type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal unsafe extern static bool DestroyComponent_EX(uint entity_id, ushort component_type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool HasComponent_EX(uint entity_id, ushort component_type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static string GetEntityName_EX(uint entity_id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static string GetEntityTag_EX(uint entity_id);

        private readonly uint MyEntityID = 0;

        public T? AddComponent<T>() where T : unmanaged
        {
            ushort type = Utils.GetComponentType<T>();
            if (type < (ushort)ComponentTypeEX.MaxEnum)
            {
                var component = (T)default;
                unsafe
                {
                    if(AddComponent_EX(&component, MyEntityID, type) == true)
                    {
                        return (T)(object)component;
                    }
                }
            }

            return null;
        }

        public T? GetComponent<T>() where T : unmanaged
        {
            ushort type = Utils.GetComponentType<T>();
            if(type < (ushort) ComponentTypeEX.MaxEnum)
            {
                var component = (T)default;
                unsafe
                {
                   if(GetComponent_EX(&component, MyEntityID, type) == true)
                   {
                       return (T)(object)component;
                   }
                }
            }

            return null;
        }

        public bool DestroyComponent<T>()
        {
            ushort type = Utils.GetComponentType<T>();
            if (type < (ushort)ComponentTypeEX.MaxEnum)
            {
                return DestroyComponent_EX(MyEntityID, type);
            }

            return false;
        }

        public bool HasComponent<T>()
        {
            ushort type = Utils.GetComponentType<T>();
            if (type < (ushort)ComponentTypeEX.MaxEnum)
            {
                return HasComponent_EX(MyEntityID, type);
            }

            return false;
        }

        public string GetName()
        {
            return GetEntityName_EX(MyEntityID);
        }

        public string GetTag()
        {
            return GetEntityTag_EX(MyEntityID);
        }

        public uint GetID()
        {
            return MyEntityID;
        }
    }
}
