using System;

namespace SmolEngine
{
    public class GameLogic : BehaviourPrimitive
    {

    }


    public class Tests
    {
        Tests()
        {

            Console.WriteLine("Im created!!");

            BehaviourPrimitive b = new BehaviourPrimitive();
            var comp = b.GetComponent<TransformComponent>();

            Console.WriteLine(comp.ToString());
        }

        private void CallMe(uint val1, uint val2)
        {
            Console.WriteLine($"Value1: {val1}, Value2: {val2}");
        }
    }
}
