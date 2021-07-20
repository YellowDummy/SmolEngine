using System;
using System.Runtime.InteropServices;

namespace SmolEngine
{
    public class Actor
    {
        public Actor()
        {
            SLog.WriteLine("Created new actor!", LogLevel.Warn);
        }

        // Properties

        private readonly uint MyEntityID = 0;

        // Methods
        public T GetComponent<T>()
        {
            ushort type = Utils.GetComponentType<T>();

            if (typeof(T) == typeof(TransformComponent))
            {
                TransformComponent component;
                unsafe
                {
                    bool found = CppAPI.GetComponent(&component, MyEntityID, type);
                    if (found)
                    {
                        return (T)(object)component;
                    }
                }
            }

            return (T)default;
        }

        public bool HasComponent<T>()
        {
            ushort type = Utils.GetComponentType<T>();

            if (typeof(T) == typeof(TransformComponent))
            {
                return CppAPI.HasComponent(MyEntityID, type);
            }

            return false;
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
