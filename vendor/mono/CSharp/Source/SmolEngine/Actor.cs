using System;
using System.Collections.Generic;

namespace SmolEngine
{
    public class Actor
    {
        // Properties

        private readonly uint ID = 0;

        public Actor(uint _id)
        {
            ID = _id;
            Console.WriteLine(ID);
            Console.WriteLine("It's work!!!!!");
        }

        ~Actor()
        {
            Console.WriteLine("Destroed!");
        }

        // Methods

        public T GetComponent<T>()
        {
            if (typeof(T) == typeof(TransformComponent))
            {
                var comp = new TransformComponent();
                bool is_exists = CppAPI.GetSetTransformComponent(ref comp, ID, false);
                if(is_exists)
                {
                    return (T)(object)comp;
                }
            }

            if (typeof(T) == typeof(HeadComponent))
            {
                var comp = new HeadComponent();
                comp.Name = "None";
                comp.Name = "None";
                bool is_exists = CppAPI.GetSetHeadComponent(ref comp, ID, false);
                if(is_exists)
                {
                    return (T)(object)comp;
                }
            }

            return (T)default;
        }

        public bool HasComponent<T>()
        {
            return false;
        }
    }
}
