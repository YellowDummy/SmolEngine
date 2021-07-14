using System;
using System.Runtime.CompilerServices;

namespace SmolEngine
{
    public class Actor
    {
        public Actor(uint _id)
        {
            ID = _id;
            Console.WriteLine("kekus");
        }

        // Methods

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void GetTransform(TransformComponent comp);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void GetHead(HeadComponent comp, uint ID);


        // Properties

        private readonly uint ID = 0;

        public T GetComponent<T>()
        {
            dynamic obj = null;

            obj = new TransformComponent();
            GetTransform(obj);

            return (T)obj;
        }
    }
}
