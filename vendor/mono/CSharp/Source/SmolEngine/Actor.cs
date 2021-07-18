using System;

namespace SmolEngine
{
    public class Actor
    {
        public Actor()
        {
            Console.WriteLine("Created new actor");
        }

        // Properties

        private readonly uint ID = 0;

        // Methods
        public T GetComponent<T>()
        {
            if (typeof(T) == typeof(TransformComponent))
            {
                var comp = new TransformComponent();
                bool is_exists = CppAPI.GetSetTransformComponent(ref comp, ID, false);
                if (is_exists)
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
                if (is_exists)
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
