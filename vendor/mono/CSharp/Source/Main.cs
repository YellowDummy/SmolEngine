using System;

namespace SmolEngine
{
    public class GameLogic : BehaviourPrimitive
    {
        private void OnBegin()
        {

        }

        private void OnUpdate()
        {

        }

        private void OnDestroy()
        {

        }

        // Optional
        private void OnCollisionBegin(uint another_id, bool is_trigger)
        {

        }

        private void OnCollisionEnd(uint another_id, bool is_trigger)
        {

        }
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
