using System;

namespace SmolEngine
{
    public class Test
    {
        Test()
        {
            Console.WriteLine("Im created!");
        }
    }

    public class Game
    {
        static void Main(string[] args)
        {
            Console.WriteLine("This main is being called from CPP");

            Actor a = new Actor(22);
            var transform = a.GetComponent<TransformComponent>();
            var head = a.GetComponent<HeadComponent>();

            Console.WriteLine(head.Name);
            Console.WriteLine(head.Tag);
            Console.WriteLine(transform.ToString());
        }

    }
}
