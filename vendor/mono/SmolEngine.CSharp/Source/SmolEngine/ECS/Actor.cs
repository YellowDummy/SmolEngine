using System;
using System.Reflection;
using System.Runtime.InteropServices;

namespace SmolEngine
{
    public class Actor
    {
        private readonly uint MyEntityID = 0;

        public Actor()
        {
            SLog.WriteLine("Created new actor!", LogLevel.Warn);
        }

        public T GetComponent<T>() where T : unmanaged
        {
            ushort type = Utils.GetComponentType<T>();
            var component = (T)default;

            unsafe
            {
                bool found = CppAPI.GetComponent(&component, MyEntityID, type);
                if (found)
                {
                    return (T)(object)component;
                }
            }

            return (T)default;
        }

        public bool SetComponent<T>(T value) where T : unmanaged
        {
            ushort type = Utils.GetComponentType<T>();
            unsafe
            {
                return CppAPI.SetComponent(&value, MyEntityID, type);
            }
        }

        public bool HasComponent<T>()
        {
            ushort type = Utils.GetComponentType<T>();
            return CppAPI.HasComponent(MyEntityID, type);
        }

        public string GetName()
        {
            return CppAPI.GetEntityName(MyEntityID);
        }

        public string GetTag()
        {
            return CppAPI.GetEntityTag(MyEntityID);
        }

        public uint GetID()
        {
            return MyEntityID;
        }
    }
}
